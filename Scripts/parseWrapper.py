import subprocess




def parse(parseString):
	f = open("parse_input.txt",'w')
	f.write(parseString)
	f.close()
	parse = subprocess.check_output(["../bin/parse_text -dir ../Grammar -grammar MGram.net -extract 1"],shell = True)
	parse = parse.strip()
	#print("Parse is as follows:\n")
	#print(":" + parse + ":")
	return parse

# Test the parse structure

#print(parse("What is Dr. Nyberg's Email Address?"))
#print(parse("What is Dr. Nyberg's email?"))

print("\n")
parseOut = (parse("Can you please tell me what Dr. Nyberg's Email is?"))
# print (parseOut)


parses = parseOut.split("\n")
# Now separate the important information from the parse.

for parse in parses:
	print(parse)

