use std::{fs, path::Path};

pub fn modify_svg(file_name: &str) -> Result<String, std::io::Error> {
    let file_path = Path::new(file_name);
    let contents = fs::read_to_string(file_path)?;
    Ok(contents)
}