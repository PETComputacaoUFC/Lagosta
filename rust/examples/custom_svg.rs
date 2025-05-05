use regex::Regex;
use rust::*;

fn main() {
    let svg_content = modify_svg("../resources/SVGgabarito_automatico.svg").unwrap();
    let re = Regex::new(r#"(data:image\/png;base64,)[^"]*"#).unwrap();
    let modified_svg: std::borrow::Cow<'_, str> = re.replace_all(&svg_content, "teste");
    println!("{}", modified_svg);
}