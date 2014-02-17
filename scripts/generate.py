import math;
import os;
import random;

datasetName = "modaltrajectory";
path = "./test/" + datasetName;

testing = path + "/testing"
reference = path + "/reference"

testing_normal = testing + "/normal"
reference_normal = reference + "/normal"

testing_bad = testing + "/1"
reference_bad = reference + "/1"

param_names = "time;row_1;"

random.seed(41);

def getValue(i, l, b,k,n,offsetX, offsetY):
	c = 2*l-((i + offsetX) / 2) % (4* l);
	coef = 1 if c >= 0 else -1;
	return coef * math.fabs(-math.fabs(((k * (i + offsetX))  % (2*l)) - l)+l +b) + offsetY;

def normalTraject(l, b, k, n, offsetX, offsetY, filename):
	f = open(filename, "w")
	print(param_names, file = f);
	for i in range(n):
		value = getValue(i, l, b, k, n,offsetX, offsetY);
		print("{0};{1};".format(i + 1, value).replace(".", ","), file = f);
	f.close();

def badTraject(l,b,k,n,offsetX, offsetY, filename, errorsCount):
	zerosCount = int((n - 4 * l * (errorsCount)) / (2 *l));

	errorsPos = [];

	for i in range(errorsCount):
		errorsPos.append(random.randint(2, zerosCount) * 2 * l - offsetX);
	
	f = open(filename, "w")
	print(param_names, file = f);

	counter = 1;

	for i in range(n - errorsCount * 2 * l):		
		if ((i) in errorsPos):
			for j in range(2*l):				
				value = getValue(i - 2*l + j, l, b, k, n, offsetX, offsetY);
				print("{0};{1};".format(counter, value).replace(".", ","), file = f);
				counter += 1;

		value = getValue(i, l,b,k,n,offsetX, offsetY);
		print("{0};{1};".format(counter, value).replace(".", ","), file = f);
		counter += 1;
	f.close();

def initEnv():
	paths = [path, testing, reference, testing_normal, reference_normal, testing_bad, reference_bad];

	for p in paths:
		if not os.path.exists(p):
			os.makedirs(p)

	f1 = open(datasetName + ".conf", "w");	
	f1.close();

	f1 = open("class_names", "w");
	print("1;", file = f1);
	f1.close();

	f1 = open("param_names", "w");
	print(param_names, file = f1);
	f1.close();

#create folters
initEnv();

# generate normal trajectories

for i in range(100):
	normalTraject(random.randint(2, 20), 0, 1, 400, random.randint(0, 100), random.randint(-30, 30), reference_normal + "/r{0}.csv".format(i+1));

for i in range(200):
	normalTraject(random.randint(2, 20), 0, 1, 800, random.randint(0, 100), random.randint(-30, 30), testing_normal + "/r{0}.csv".format(i+1));

# generate bad trajectories

for i in range(100):
	badTraject(random.randint(2, 20), 0, 1, 400, random.randint(0, 100), random.randint(-30, 30), reference_bad + "/r{0}.csv".format(i+1), 1);

for i in range(200):
	badTraject(random.randint(2, 20), 0, 1, 800, random.randint(0, 100), random.randint(-30, 30), testing_bad + "/r{0}.csv".format(i + 1), 1);


