import numpy as np
import pandas as pd
import os
import os.path

path = 'UCR_TS_Archive_2015'
new_path = 'UCR_axiomlib'

os.mkdir(new_path)

datasets = os.listdir(path)

for dataset in datasets:
    print 'Processing dataset: {d}'.format(d=dataset)
    dataset_dir = os.path.join(new_path, dataset)
    print 'Creating dir: {d}'.format(d=dataset_dir)
    os.mkdir(dataset_dir)
    print 'Creating dirs for train and test'
    os.mkdir(os.path.join(dataset_dir, 'reference'))
    os.mkdir(os.path.join(dataset_dir, 'testing'))
    
    train_name = dataset + '_TRAIN'
    test_name = dataset + '_TEST'
    
    train_file = pd.read_csv(os.path.join(path, dataset, train_name), header=None)
    test_file = pd.read_csv(os.path.join(path, dataset, test_name), header=None)
    
    y_train = train_file[0].values
    X_train = train_file[train_file.columns[1:]].values
    y_test = test_file[0].values
    X_test = test_file[test_file.columns[1:]].values
    
    classes = np.unique(y_train)
    normal_class = classes[0]
    other_classes = classes[1:]
    
    param_names = ['time', 'row_1']
    delta = other_classes[0] - 1
    class_names = list(np.asarray(other_classes - delta, dtype=str))
    
    print 'Creating file with class names'
    f = open(os.path.join(dataset_dir, 'class_names'), 'w')
    f.write(';'.join(class_names) + ';')
    f.close()
    
    print 'Creating file with param names'
    f = open(os.path.join(dataset_dir, 'param_names'), 'w')
    f.write(';'.join(param_names) + ';')
    f.close()
    
    print 'Creating config file'
    f = open(os.path.join(dataset_dir, dataset + '.conf'), 'w')
    f.write('//Dataset parameters\n')
    f.write("    null:\t'null'\n")
    f.write("    analizeParameter:\t'row_1'\n")
    f.write("// class name in referenceClasses for normal behaviour\n")
    f.write("    normalClass:\t'normal'\n")
    f.close()
    
    print 'Creating files with train dataset'
    os.mkdir(os.path.join(dataset_dir, 'reference', 'normal'))
    i = 0
    for row in X_train[y_train == normal_class]:
        df = pd.DataFrame({'time':np.arange(len(row))+1, 'row_1':row})
        df = df[['time', 'row_1']]
        df.to_csv(os.path.join(dataset_dir, 'reference', 'normal', 'ref'+str(i).zfill(4)+'.csv'), sep=';', line_terminator=';\n', index=False)
        i += 1
    
    for cl in other_classes:
        os.mkdir(os.path.join(dataset_dir, 'reference', str(cl - delta)))
        i = 0
        for row in X_train[y_train == cl]:
            df = pd.DataFrame({'time':np.arange(len(row))+1, 'row_1':row})
            df = df[['time', 'row_1']]
            df.to_csv(os.path.join(dataset_dir, 'reference', str(cl - delta), 'ref'+str(i).zfill(4)+'.csv'), sep=';', line_terminator=';\n', index=False)
            i += 1
    
    print 'Creating files with test dataset'
    os.mkdir(os.path.join(dataset_dir, 'testing', 'normal'))
    i = 0
    for row in X_test[y_test == normal_class]:
        df = pd.DataFrame({'time':np.arange(len(row))+1, 'row_1':row})
        df = df[['time', 'row_1']]
        df.to_csv(os.path.join(dataset_dir, 'testing', 'normal', 'ref'+str(i).zfill(4)+'.csv'), sep=';', line_terminator=';\n', index=False)
        i += 1
    
    for cl in other_classes:
        os.mkdir(os.path.join(dataset_dir, 'testing', str(cl - delta)))
        i = 0
        for row in X_test[y_test == cl]:
            df = pd.DataFrame({'time':np.arange(len(row))+1, 'row_1':row})
            df = df[['time', 'row_1']]
            df.to_csv(os.path.join(dataset_dir, 'testing', str(cl - delta), 'ref'+str(i).zfill(4)+'.csv'), sep=';', line_terminator=';\n', index=False)
            i += 1
