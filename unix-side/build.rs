use std::process::Command;

const PI_SIDE: &str = "../pi-side";

fn main() {
    println!("cargo:rerun-if-changed={}", PI_SIDE);

    let status = Command::new("make")
        .current_dir(PI_SIDE)
        .status().expect("Failed to run make");
    if !status.success() {
        panic!("make failed");
    }
}