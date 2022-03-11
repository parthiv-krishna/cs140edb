use std::fs::read_dir;

const DEV_PATH: &str = "/dev/";

const PREFIXES: [&str; 3] = [
    "ttyUSB",	// linux
	"cu.SLAB_USB", // mac os
    "cu.usbserial", // for esp programmer
];

pub fn find_tty() -> Option<String> {
    let dir = read_dir(DEV_PATH).expect("Failed to read /dev/");
    let filename = dir.filter_map(|file| file.ok()).find(|file| {
        PREFIXES.iter().any(|pre| file.file_name().to_str().unwrap().starts_with(pre))
    })?.file_name().to_str().unwrap().to_owned();
    return Some(DEV_PATH.to_owned() + &filename);
}