use std::fs::{read_dir, File};
use std::io::Result;
use std::os::unix::prelude::AsRawFd;

use termios::os::target::CRTSCTS;
use termios::{Termios, cfsetspeed, IGNBRK, VMIN, PARENB, CSTOPB, CSIZE, CREAD, CLOCAL, IXON, IXOFF, IXANY, ICANON, ECHO, ECHOE, ISIG, OPOST, CS8, TCSANOW, tcsetattr, VTIME};

const DEV_PATH: &str = "/dev/";

const PREFIXES: [&str; 3] = [
    "ttyUSB",	// linux
	"cu.SLAB_USB", // mac os
    "cu.usbserial", // for esp programmer
];

pub fn find_tty_device_file() -> Option<String> {
    let dir = read_dir(DEV_PATH).expect("Failed to read /dev/");
    let filename = dir.filter_map(|file| Some(file.ok()?.file_name().to_str()?.to_owned()))
        .find(|fname| PREFIXES.iter().any(|pre| fname.starts_with(pre)))?;
    return Some(DEV_PATH.to_owned() + &filename);
}

pub fn setup_tty(file: &File) -> Result<()> {
    let fd = file.as_raw_fd();
    let mut tty = Termios::from_fd(fd)?;
    // disable IGNBRK for mismatched speed tests; otherwise receive break
    // as \000 chars

    cfsetspeed(&mut tty, 115200)?;
    // XXX: wait, does this disable break or ignore-ignore break??
    tty.c_iflag &= !IGNBRK;         // disable break processing
    tty.c_lflag = 0;                // no signaling chars, no echo,
    // no canonical processing
    tty.c_oflag = 0;                // no remapping, no delays
    tty.c_cc[VMIN]  = 0;            // read doesn't block
    tty.c_cc[VTIME] = 2;

	/*
	 * Setup TTY for 8n1 mode, used by the pi UART.
	 */

    // Disables the Parity Enable bit(PARENB),So No Parity 
    tty.c_cflag &= !PARENB; 	
    // CSTOPB = 2 Stop bits,here it is cleared so 1 Stop bit 
    tty.c_cflag &= !CSTOPB;   	
    // Clears the mask for setting the data size     
    tty.c_cflag &= !CSIZE;	 	
    // Set the data bits = 8
    tty.c_cflag |=  CS8; 		
    // No Hardware flow Control 
    tty.c_cflag &= !CRTSCTS;
    // Enable receiver,Ignore Modem Control lines 
    tty.c_cflag |= CREAD | CLOCAL; 	
    	
    // Disable XON/XOFF flow control both i/p and o/p
    tty.c_iflag &= !(IXON | IXOFF | IXANY);    
    // Non Cannonical mode 
    tty.c_iflag &= !(ICANON | ECHO | ECHOE | ISIG);  
    // No Output Processing
    tty.c_oflag &= !OPOST;	
    tcsetattr(fd, TCSANOW, &tty)?;
    Ok(())
}