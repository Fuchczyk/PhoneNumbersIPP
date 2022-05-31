use rand::prelude::*;
use std::collections::HashMap;
use std::collections::BTreeSet;
use std::env;

const MIN_SIZE: usize = 2;
const MAX_SIZE: usize = 20;

struct StringWrapper {
    wrapped: String,
}

impl StringWrapper {
    pub fn new(string: String) -> StringWrapper {
        StringWrapper { wrapped: string }
    }
}

impl PartialEq for StringWrapper {
    fn eq(&self, other: &Self) -> bool {
        self.wrapped == other.wrapped
    }
}

impl Eq for StringWrapper {}

impl PartialOrd for StringWrapper {
    fn partial_cmp(&self, other: &Self) -> Option<std::cmp::Ordering> {
        let mut chars_me = self.wrapped.chars();
        let mut chars_other = other.wrapped.chars();

        let len_self = self.wrapped.len();
        let len_other = other.wrapped.len();

        let len: usize;
        if len_other < len_self {
            len = len_other;
        } else {
            len = len_self;
        }

        let mut char_me = '1';
        let mut char_other = '1';

        let mut difference = false;
        for _i in 0..len {
            char_me = chars_me.next().unwrap();
            char_other = chars_other.next().unwrap();

            if char_me != char_other {
                difference = true;
                break;
            }
        }

        if !difference {
            if len_self == len_other {
                return Some(std::cmp::Ordering::Equal);
            } else if len_self > len_other {
                return Some(std::cmp::Ordering::Greater);
            } else {
                return Some(std::cmp::Ordering::Less);
            }
        } else {
            let digitize_me = digitize_char(char_me);
            let digitize_other = digitize_char(char_other);

            if digitize_me > digitize_other {
                Some(std::cmp::Ordering::Greater)
            } else if digitize_me == digitize_other {
                Some(std::cmp::Ordering::Equal)
            } else {
                Some(std::cmp::Ordering::Less)
            }
        }
    }
}

impl Ord for StringWrapper {
    fn cmp(&self, other: &Self) -> std::cmp::Ordering {
        self.partial_cmp(other).unwrap()
    }
}

fn digitize_char(number: char) -> u8 {
    match number {
        '*' => 10,
        '#' => 11,
        otherwise => {
            let x = otherwise.to_digit(10);

            x.unwrap() as u8
        }
    }
}

fn digitize_number(number: i32) -> String {
    match number {
        10 => "*".to_owned(),
        11 => "#".to_owned(),
        other => other.to_string(),
    }
}

fn random_sequence(rng: &mut ThreadRng) -> String {
    let len: usize = rng.gen_range(MIN_SIZE..=MAX_SIZE);
    let mut res = String::new();

    for _i in 0..len {
        res.push_str(&digitize_number(rng.gen_range(0..=11)));
    }

    res
}

fn one_is_prefix_of_two(pref: &str, num: &str) -> bool {
    if pref.len() > num.len() {
        return false;
    }

    let mut pref_it = pref.chars();
    let mut num_it = num.chars();

    while let Some(c) = pref_it.next() {
        let c2 = num_it.next().unwrap();
        if c != c2 {
            return false;
        }
    }

    true
}

fn remove_map(num: &str, map: &mut HashMap<String, String>) {
    let mut vec: Vec<String> = Vec::new();

    for key in map.keys() {
        if one_is_prefix_of_two(num, key) {
            vec.push(key.to_string());
        }
    }

    for element in vec {
        map.remove(&element);
    }
}

fn gen_removed(rng: &mut ThreadRng, map: &mut HashMap<String, String>) {
    let x;
    if rng.gen_bool(0.7) && !map.is_empty() {
        x = map.keys().next().unwrap().to_string();
    } else {
        x = random_sequence(rng);
    }

    remove_map(&x, map);
    println!("REMOVE {}", x);
}

fn gen_get(rng: &mut ThreadRng, map: &HashMap<String, String>) {
    let mut pref = String::new();
    let mut pair: Option<&str> = None;
    let mut pref_len: usize = 0;

    let x = random_sequence(rng);

    for c in x.chars() {
        pref.push(c);
        if let Some(p) = map.get(&pref) {
            pair = Some(p);
            pref_len = pref.len();
        }
    }

    if pair.is_none() {
        println!("GET {} {}", x, x);
    } else {
        let mut p = String::from(pair.unwrap());
        let suf: String = x.chars().skip(pref_len).collect();
        p.push_str(&suf);

        println!("GET {} {}", x, p);
    }
}

fn gen_addition(rng: &mut ThreadRng, map: &mut HashMap<String, String>) {
    let num1 = random_sequence(rng);
    let num2 = random_sequence(rng);

    println!("ADD {} {}", num1, num2);
    if !num1.eq(&num2) {
        map.insert(num1, num2);
    }
}

fn reverse_addition(key: &str, prefix: &str, origin: &str) -> String {
    let len = prefix.len();

    let mut rev_result = String::new();
    rev_result.push_str(key);

    let orig_chars = origin.chars();
    let skipped = orig_chars.skip(len);
    let skip_result: String = skipped.collect();

    rev_result.push_str(&skip_result);
    rev_result
}

fn gen_reverse(rng: &mut ThreadRng, map: &HashMap<String, String>) {
    let x = random_sequence(rng);

    let mut prefix = String::new();
    let mut x_chars = x.chars();

    prefix.push(x_chars.next().unwrap());

    let mut results: BTreeSet<StringWrapper> = BTreeSet::new();

    for c in x_chars {
        prefix.push(c);

        for (key, value) in map.iter() {
            if prefix.eq(value) {
                results.insert(StringWrapper::new(reverse_addition(key, &prefix, &x)));
            }
        }
    }
    let wrapper = StringWrapper::new(x.clone());
    results.insert(wrapper);

    println!("REVERSE {}", x);

    for elem in results {
        println!("GETREVERSE {}", elem.wrapped);
    }

    println!("REVERSE_END");
}

fn main() {
    let mut rng = thread_rng();
    let mut database: HashMap<String, String> = HashMap::new();

    let tests: Result<usize, _> = env::args()
        .into_iter()
        .skip(1)
        .next()
        .unwrap()
        .to_string()
        .parse();
    let tests = tests.unwrap();
    for i in 0..tests {
        if i % 10000 == 0 {
            eprintln!("STAN: {}", i as f64 / tests as f64);
        }
        if rng.gen_bool(0.6) {
            gen_addition(&mut rng, &mut database);
        } else if rng.gen_bool(0.15) {
            gen_get(&mut rng, &database);
        } else if rng.gen_bool(0.4) {
            gen_removed(&mut rng, &mut database);
        } else {
            gen_reverse(&mut rng, &database);
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn test_comparing() {
        let s1 = StringWrapper::new("*#6*544#".to_string());
        let s2 = StringWrapper::new("*344#".to_string());

        assert_eq!(s1.partial_cmp(&s2).unwrap() , std::cmp::Ordering::Greater);
    }
}