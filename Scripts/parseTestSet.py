import subprocess

# NOTE: in test set, // indicates a comment line, and # represents
# a printing comment.  That is, comments marked with # are copied to the 
# log file along with the parses.  This is useful for labeling sections.


testFileName = "testSetQuestions.txt"
testFileOutName = "testSetParses.txt"


def parse(parseString):
	f = open("parse_input.txt",'w')
	f.write(parseString)
	f.close()
	parse = subprocess.check_output(["../bin/parse_text -dir ../Grammar -grammar MGram.net -extract 1"],shell = True)
	parse = parse.strip()
	#print("Parse is as follows:\n")
	#print(":" + parse + ":")
	return parse

# Test the parse struct

print("Parsing Test Data From File: " + testFileName + "\nUsing log file: " + testFileOutName)
testSet = open(testFileName,'r')
log = open(testFileOutName,'w')

for question in testSet:
	#remove blank lines and comments
	if (len(question.strip()) < 1) or (question.strip().startswith('//')):
		continue
	if question.strip().startswith('#'):
		log.write(question)
		print(question)
		continue
	#parse text
	p = parse(question.strip())
	print(p)
	log.write(question)
	log.write(p)
	log.write("\n\n")

print("\n")
#parseOut = (parse("Can you please tell me what Dr. Nyberg's Email is?"))
# print (parseOut)


#parses = parseOut.split("\n")
# Now separate the important information from the parse.

#for parse in parses:
#	print(parse)

