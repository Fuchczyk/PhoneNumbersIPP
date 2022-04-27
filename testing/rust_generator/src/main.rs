use rand::prelude::*;
use std::env;
use std::collections::HashMap;

const MIN_SIZE: usize = 1;
const MAX_SIZE: usize = 1;

fn random_sequence(rng: &mut ThreadRng) -> String {
    let len: usize = rng.gen_range(MIN_SIZE..=MAX_SIZE);
    let mut res = String::new();

    for _i in 0..len {
        res.push_str(&rng.gen_range(0..=9).to_string());
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
    map.insert(num1, num2);
}



fn main() {
    let mut rng = thread_rng();
    let mut database: HashMap<String, String> = HashMap::new();
    
    let tests: Result<usize, _> = env::args().into_iter().skip(1).next().unwrap().to_string().parse();
    let tests = tests.unwrap();
    for i in 0..tests {
        if i % 10000 == 0 {
            eprintln!("STAN: {}", i as f64 / tests as f64);
        }
        if rng.gen_bool(0.5) {
            gen_addition(&mut rng, &mut database);
        } else if rng.gen_bool(0.5) {
            gen_get(&mut rng, &database);
        } else {
            gen_removed(&mut rng, &mut database);
        }
    }
}
