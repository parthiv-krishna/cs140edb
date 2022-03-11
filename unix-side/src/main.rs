use std::env::args;
use std::fs::File;
use std::io::{Read, self};
use std::path::Path;

use cs140edb::pi_echo::pi_echo;
use cs140edb::simple_boot::simple_boot;
use cs140edb::tty::{find_tty_device_file, setup_tty};

fn create_file_buffer<P: AsRef<Path>>(in_file: P) -> io::Result<Vec<u8>> {
    let debugger_bin = include_bytes!("../../pi-side/debugger.bin");
    let mut in_bin = File::open(&in_file)?;
    let len: u64 = in_bin.metadata()?.len();

    let mut buf = debugger_bin.to_vec();
    buf.extend(&u32::try_from(len).expect("File too large").to_le_bytes());
    let mut buf = Vec::new();
    in_bin.read_to_end(&mut buf)?;

    Ok(buf)
}

fn main() {
    let argv: Vec<_> = args().skip(1).collect();

    let (tty_file, in_file) = match argv.len() {
        1 => (find_tty_device_file().expect("Found 0 ttys"), &argv[0]),
        2 => (argv[0].clone(), &argv[1]),
        _ => panic!("Expected 1 or two arguments")
    };

    let mut tty = File::options().read(true).write(true).open(tty_file).expect("Failed to open tty");
    setup_tty(&tty).expect("Failed to setup tty");

    let buf = create_file_buffer(&in_file).expect("Failed to read input file");

    simple_boot(&mut tty, &buf).expect("Failed to send code to pi");
    println!("Finished loading program");
    pi_echo(&mut tty).expect("Failed to echo stdin");
}
