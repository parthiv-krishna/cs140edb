use std::fmt::Display;
use std::fs::File;
use std::io::{self, Read, Write};

use crate::crc::crc32;

const ARMBASE: u32 = 0x8000;

// unix sends to pi
const PUT_PROG_INFO: u32 = 0x33334444;
const PUT_CODE: u32 = 0x77778888;

// pi sends to unix
const GET_PROG_INFO: u32 = 0x11112222;
const GET_CODE: u32 = 0x55556666;
const BOOT_SUCCESS: u32 = 0x9999AAAA;

const PRINT_STRING: u32 = 0xDDDDEEEE; // pi sends to print a string.

// error codes from the pi to unix
#[allow(dead_code)] 
const BOOT_ERROR: u32 = 0xBBBBCCCC; // pi sends on failure.
#[allow(dead_code)] 
const BAD_CODE_ADDR: u32 = 0xdeadbeef;
#[allow(dead_code)] 
const BAD_CODE_CKSUM: u32 = 0xfeedface;


fn get8(tty: &mut File) -> io::Result<u8> {
    let mut buf = [0; 1];
    while tty.read(&mut buf)? == 0 {   
    }
    Ok(buf[0])
}

fn read_exact(tty: &mut File, buf: &mut [u8]) -> io::Result<()> {
    for i in 0..buf.len() {
        buf[i] = get8(tty)?;
    }
    Ok(())
}

fn get32(tty: &mut File) -> io::Result<u32> {
    let mut buf = [0; 4];
    read_exact(tty, &mut buf)?;
    let res = u32::from_le_bytes(buf);
    // println!("GET32: {:X}", res);
    Ok(res)
}

fn put32(tty: &mut File, x: u32) -> io::Result<()> {
    // println!("PUT32: {:X}", x);
    tty.write_all(&mut x.to_le_bytes())
}

fn process_op(tty: &mut File) -> io::Result<u32> {
    Ok(loop {
        let op = get32(tty)?;
        if op != PRINT_STRING {
            break op;
        }
        let len = get32(tty)? as usize;
        if len >= 512 {
            panic!("Recieved suspiciously long print string!");
        }
        let mut buf = vec![0; len];
        read_exact(tty, &mut buf)?;
        let mut s = String::from_utf8(buf).expect("Invalid string recieved");
        match s.chars().last() {
            Some(c) if c == '\n' => {
                s.pop();
            }
            _ => {}
        }
        println!("pi sent print: <{}>", s);
    })
}

fn check_data<S: AsRef<str> + Display>(expected: u32, recieved: u32, msg: S) {
    if expected != recieved {
        panic!("{}. Expected {:X}, received {:X}", msg, expected, recieved);
    }
}

pub fn simple_boot(tty: &mut File, buf: &[u8]) -> io::Result<()> {
    let crc = crc32(buf, 0);
    println!("cs140edb: sending {} bytes, crc={:x}", buf.len(), crc);
    println!("waiting for start");

    // loop {
    //     println!("{:X}", get8(tty)?);
    // }

    loop {
        let op = process_op(tty)?;
        if op == GET_PROG_INFO {
            break;
        }
        println!(
            "expected initial GET_PROG_INFO, got <{:X}>: discarding.",
            op
        );
        get8(tty)?;
    }
    put32(tty, PUT_PROG_INFO)?;
    put32(tty, ARMBASE)?;
    put32(tty, buf.len().try_into().expect("Program is too large"))?;
    put32(tty, crc)?;

    check_data(GET_CODE, loop {
        match process_op(tty)? {
            GET_PROG_INFO => continue,
            x => break x,
        }
    }, "GET_CODE mismatch");
    check_data(crc,  process_op(tty)?, "crc mismatch");
    put32(tty, PUT_CODE)?;
    tty.write_all(buf)?;
    check_data(BOOT_SUCCESS, process_op(tty)?, "BOOT_SUCCESS mismatch");
    Ok(())
}
