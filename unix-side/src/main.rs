use std::env::args;
use std::fs::File;
use std::io::{Write, Read};

fn main() {
    let debugger_bin = include_bytes!("../../pi-side/debugger.bin");

    let in_file = args().skip(1).next().expect("Expected one argument, got 0");
    let mut in_bin = File::open(&in_file).expect(&format!("Failed to open {}", &in_file));
    let len: u64 = in_bin.metadata().expect("Failed to get file metadata").len();

    let mut out_file = File::create("out.bin").expect("Failed to create out file");
    
    out_file.write_all(debugger_bin).expect("Failed to write to out file");
    out_file.write_all(&u32::try_from(len).expect("File too large").to_le_bytes())
        .expect("Failed to write to out file");
    let mut buf = Vec::new();
    in_bin.read_to_end(&mut buf).expect("Failed to read input binary");
    out_file.write_all(&buf).expect("Failed to write to out file");
}
