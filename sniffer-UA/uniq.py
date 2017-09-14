#!/usr/bin/env python
# encoding: utf-8

f_before = open('ua.txt', 'r')
f_after = open('ua-uniq.txt', 'w')
before = f_before.read().split('\n')
after = list(set(before))

for i in after:
    f_after.write(i)
    f_after.write('\n')

f_before.close()
f_after.close()
