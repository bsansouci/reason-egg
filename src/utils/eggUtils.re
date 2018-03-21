let optionMap = (f, option: option('a)) => switch (option) {
    | Some(x) => Some(f(x))
    | _ => None
    };

let find = (f, list) => {
        let found = List.filter(f, list);
        (List.length(found) === 0) ? None : Some(List.hd(found));
      };