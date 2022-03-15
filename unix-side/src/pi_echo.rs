use std::fs::File;
use std::io::{ self, Write, Read, stdout, stdin};
use std::thread;
use std::sync::mpsc::{SyncSender, sync_channel};

pub fn pi_echo(tty: &mut File) -> io::Result<()> {
    let mut output = stdout();
    let mut buf = [0; 64];

    let (sender, receiver) = sync_channel(10);

    thread::spawn(move || read_stdin(sender));
    loop {
        let n = tty.read(&mut buf)?;
        output.write_all(&buf[0..n]).expect("Failed to write to stdout");
        output.flush().expect("Failed to write to stdout");
        // TODO: Look for DONE!!!
        for line in receiver.try_iter() {
            tty.write_all(line.as_bytes())?;
        }
    }
    // Ok(())
}

fn read_stdin(sender: SyncSender<String>) {
    let input = stdin();
    
    loop {
        let mut line = String::new();
        input.read_line(&mut line).expect("Failed to read from stdin");
        sender.send(line).expect("Failed to transfer line");
    }
}