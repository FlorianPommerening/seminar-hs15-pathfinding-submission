#!/usr/bin/env python

import sys
import glob

def handle_dir(dirname):
	for f in sorted(glob.glob(dirname + "/*.log")):

		# scen total-time 0.0 max-time-step 0.0 time-20-moves 0.0 total-len 0.0 subopt 0.0 valid
		num_lines = 0
		time = 0
		subopt = 1
		for line in open(f):
			tokens = line.strip().split("\t")
			if len(tokens) == 12:
				time += float(tokens[2])
				subopt += float(tokens[10])
				assert tokens[11] == "valid", tokens[11]
				num_lines += 1
		print f, num_lines
		print "total time: ", round(time, 1)
		print "geo mean(subopt): ", round(subopt / num_lines, 2)


handle_dir(sys.argv[1])
