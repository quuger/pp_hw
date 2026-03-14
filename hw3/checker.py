s = input()
s = s.split(" ")
s = s[:-1]
for a in s:
    h = 0
    o = 0
    for x in a:
        if x == "H":
            h += 1
        else:
            o += 1
    if h != 2 or o != 1:
        print(a)
        raise Exception
