import math;
import os;
import random;

datasetName = "new";
path = "./test/" + datasetName;

testing = path + "/testing"
reference = path + "/reference"

testing_normal = testing + "/normal"
reference_normal = reference + "/normal"

testing_bad = testing + "/1"
reference_bad = reference + "/1"

param_names = "time;row_1;"


conf = """//DataSet Parameters
    null:		'null'
    analizeParameter:	'row_1'
// class name in referenceClasses for normal befaviur
    normalClass:	'normal' 
"""

def peak(min, max, count):
	step = (max - min) / count

	val = min
	result = []

	for i in range(int(min), int(max) + 1):
		result.append(val)
		val += step

	val -= step

	for i in range(int(min), int(max)):
		val -= step
		result.append(val)

	return result

def antipeak(max, min, count):
	step = (max - min) / count

	val = max
	result = []

	for i in range(int(min), int(max) + 1):
		result.append(val)
		val -= step
	
	val += step

	for i in range(int(min), int(max)):
		val += step
		result.append(val)

	return result

# min - минимальное значение 
# max - максимальное значение
# count - количество пиков
# len - количество точек в пике
def trajectory(min, max, count, len):
	avg = (max + min) / 2;

	result = []

	_peak = peak(avg, max, len)	
	_antipeak = antipeak(avg, min, len)

	result += _peak
	result += _peak[1: ]

	for i in range(count - 1):
		if (i % 2 == 1):
			result += _peak[1: -1]
			result += _peak
		else: 
			result += _antipeak[1: -1]
			result += _antipeak

	return result	

# min - минимальное значение 
# max - максимальное значение
# count - количество пар пиков
# len - количество точек в пике
# errorIn номер пика, в котором будет ошибка
def badtrajectory(min, max, count, len, errorIn):
	avg = (max + min) / 2;

	result = []

	_peak = peak(avg, max, len)	
	_antipeak = antipeak(avg, min, len)

	result += _peak
	result += _peak[1: ]

	if (errorIn == 1):
		result += _peak[1: ]

	for i in range(count - 1):
		if (i % 2 == 1):
			result += _peak[1: -1]
			result += _peak		

			if (errorIn == i + 2):		
				result += _peak[1:]
		else: 
			result += _antipeak[1: -1]
			result += _antipeak

			if (errorIn == i + 2):		
				result += _antipeak[1:]

	return result				

def normalTraject(min, max, count, l, filename):
	f = open(filename, "w")
	print(param_names, file = f);
	traj = trajectory(min, max, count, l)

	for i in range(len(traj)):		
		print("{0};{1};".format(i + 1, traj[i]), file = f);
	f.close();

def badTraject(min, max, count, l, errorIn, filename):
	f = open(filename, "w")
	print(param_names, file = f);
	traj = badtrajectory(min, max, count, l, errorIn)

	for i in range(len(traj)):		
		print("{0};{1};".format(i + 1, traj[i]), file = f);

	f.close();


def initEnv():
	paths = [path, testing, reference, testing_normal, reference_normal, testing_bad, reference_bad];

	for p in paths:
		if not os.path.exists(p):
			os.makedirs(p)

	f1 = open(path + '/' + datasetName + ".conf", "w");	
	print(conf, file = f1);
	f1.close();

	f1 = open(path + '/' + "class_names", "w");
	print("1;", file = f1);
	f1.close();

	f1 = open(path + '/' + "param_names", "w");
	print(param_names, file = f1);
	f1.close();

initEnv();

height = 10
count = 5
l = 5


trajCount = 50

for i in range(trajCount):
	#base = random.randint(-100, 100) * 2
	base = 0
	normalTraject(base, base  + height, count, l, reference_normal + "/r{0}.csv".format(i+1));

for i in range(trajCount):
	#base = random.randint(-100, 100) * 2
	base = 0
	normalTraject(base , base + height, count, l, testing_normal + "/r{0}.csv".format(i+1));

for i in range(trajCount):
	#base = random.randint(-100, 100) * 2
	base = 0
	badTraject(base , base + height, 1, l, 1,  reference_bad + "/r{0}.csv".format(i+1));

for i in range(trajCount):
	#base = random.randint(-100, 100) * 2
	base = 0
	badTraject(base , base + height, 1, l, 1, testing_bad + "/r{0}.csv".format(i + 1));

