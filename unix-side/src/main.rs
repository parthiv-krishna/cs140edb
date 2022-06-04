use std::borrow::Cow;
use std::env::args;
use std::fs::File;
use std::io::{Read, self};
use std::path::Path;

use cs140edb::pi_echo::pi_echo;
use cs140edb::simple_boot::simple_boot;
use cs140edb::tty::{find_tty_device_file, setup_tty};

fn create_file_buffer<P: AsRef<Path>>(in_file: P, debug: bool) -> io::Result<Vec<u8>> {
    let debugger_bin = include_bytes!("../../pi-side/debugger.bin");
    let mut in_bin = File::open(&in_file)?;
    let mut buf = if debug {
        let len = in_bin.metadata()?.len();
        let mut buf = debugger_bin.to_vec();
        buf.extend(&u32::try_from(len).expect("File too large").to_le_bytes());
        buf
    } else {
        Vec::new()
    };
    in_bin.read_to_end(&mut buf)?;
    Ok(buf)
}

fn main() {
    let mut argv: Vec<_> = args().skip(1).collect();
    let debug = if let Some("-r") = argv.first().map(String::as_str) {
        argv.remove(0);
        false
    } else {
        true
    };
    let (tty_file, in_file) = match argv.len() {
        1 => (Cow::from(find_tty_device_file().expect("Found 0 ttys")), &argv[0]),
        2 => (Cow::from(&argv[0]), &argv[1]),
        _ => panic!("Expected one or two arguments")
    };

    let mut tty = File::options()
        .read(true).write(true)
        .open(tty_file.as_ref())
        .expect("Failed to open tty");

    setup_tty(&tty).expect("Failed to setup tty");

    let buf = create_file_buffer(&in_file, debug).expect("Failed to read input file");

    simple_boot(&mut tty, &buf).expect("Failed to send code to pi");
    println!("Finished loading program");
    let msg = match pi_echo(&mut tty) {
        Ok(()) => "Saw done.",
        Err(_) => "Pi disconnected."
    };
    println!("{}", msg);
}
