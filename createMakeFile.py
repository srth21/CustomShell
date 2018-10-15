import os
import sys
args=sys.argv[1:]
mainFileName=args[0]
funcFileName=args[1]
headerFileName=args[2]

comment1="//makeFile Starts Here.\n"
#print("hello")
mainObjectFileName=(mainFileName.split('.')[0])+".o"
funcObjectFileName=(funcFileName.split('.')[0])+".o"

executableName="prog"

line0="cc=gcc\n"

line1=executableName+": "+mainObjectFileName+" "+funcObjectFileName+"\n"+"\t"+"$(cc) -o "+executableName+" "+mainObjectFileName+" "+funcObjectFileName+"\n"

#print(line1) 

line2=funcObjectFileName+": "+funcFileName+"\n"+"\t"+"$(cc) -c "+funcFileName+"\n"

line3=mainObjectFileName+": "+mainFileName+" "+headerFileName+"\n"+"\t"+"$(cc) -c "+mainFileName+" a.out"+"\n"

line4="clean : \n\trm -rf *.o\n"

code=comment1+line0+line1+line2+line3+line4
#print(code)
makeFileName="makeFileFor_"+mainFileName+"_.mk"
with open(makeFileName,"w") as f:
	f.write(code)
f.close()

print("MakeFile Name : ",makeFileName)
print("Executable Name : prog. Execute using ./prog")