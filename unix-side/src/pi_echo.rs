use std::fs::File;
use std::io::{ self, Write, Read, stdout};

pub fn pi_echo(tty: &mut File) ->io::Result<()> {
    let mut output = stdout();
    let mut buf = [0; 512];

    loop {
        let n = tty.read(&mut buf).expect("Pi disconnected");
        output.write_all(&buf[0..n])?;
    }
}