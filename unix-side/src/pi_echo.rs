use std::fs::File;
use std::io::{ self, Write, Read, stdout, stdin};
use std::thread;
use std::sync::mpsc::{SyncSender, sync_channel};

const DONE: &[u8] = "DONE!!!".as_bytes();

fn check_for_done(cur_pos: &mut usize, new_data: &[u8]) -> bool {
    for &b in new_data {
        if b == DONE[*cur_pos] {
            *cur_pos += 1;
            if *cur_pos >= DONE.len() {
                return true;
            }
        } else {
            *cur_pos = 0;
        }
    }
    return false
}

pub fn pi_echo(tty: &mut File) -> io::Result<()> {
    let mut done_tracker = 0;
    let mut output = stdout();
    let mut buf = [0; 64];

    let (sender, receiver) = sync_channel(10);

    thread::spawn(move || read_stdin(sender));
    loop {
        let n = tty.read(&mut buf)?;
        output.write_all(&buf[0..n]).expect("Failed to write to stdout");
        output.flush().expect("Failed to write to stdout");
        if check_for_done(&mut done_tracker, &buf[0..n]) {
            break Ok(())
        }

        for line in receiver.try_iter() {
            tty.write_all(line.as_bytes())?;
        }
    }
}

fn read_stdin(sender: SyncSender<String>) {
    let input = stdin();
    
    loop {
        let mut line = String::new();
        input.read_line(&mut line).expect("Failed to read from stdin");
        sender.send(line).expect("Failed to transfer line");
    }
}