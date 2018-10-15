import os
import sys

args=sys.argv[1:]

validExtensions=["txt","c","py"]
positions={}
for word in args:
	for file in os.listdir():
		if(os.path.isdir(file)==0):
			if(file.split('.')[-1] in validExtensions):
				with open(file,"r",encoding='utf-8') as f:
					data=f.read()
				f.close()
				lineNum=0
				for line in data.split("\n"):
					lineNum+=1
					if(word in line.split()):
						if(word not in positions):
							positions[word]=[]
						entry=[file,lineNum]
						positions[word].append(entry)

for word in args:
	if(word not in positions):
		print(word," not present.\n")
	else:
		entries=positions[word]
		print("Looking for the word : ",word)
		for entry in entries:
			print("Found in ",entry[0]," at Line Number ",entry[1])
