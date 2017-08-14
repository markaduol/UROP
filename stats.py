#!/usr/bin/env python3

import os
import re
import sys
import argparse

DEBUG=True

STATS_FILE = 'run.stats'
REV_FILE = 'revisions.txt'

from operator import itemgetter
#try:
#    from tabulate import TableFormat, Line, DataRow, tabulate
#except:
#    print('Error: Package "tabulate" required for table formatting. '
#          'Please install it using "pip".', 
#          file=sys.stderr)
#    exit(1)

Legend = [
    ('Instrs', 'number of executed instructions'),
    ('Time', 'total wall time (s)'),
    ('TUser', 'total user time'),
    ('Tests', 'total number of test cases generated'),
    ('ICov', 'instruction coverage in the LLVM bitcode (%)'),
    ('BCov', 'branch coverage in the LLVM bitcode'),
    ('Function', 'base name of function under test'),
    ('Category', 'whether function added, removed or modified'),
    ('Revisions', 'revision pair in use')
]

##################### DEBUG FUNCTIONS ######################

def PRINT(val):
    if DEBUG:
        print(val) 

############################################################

def getAbsPath(path):
    return os.path.abspath(path)

def getRevisionLog(path):
    """Return the relevant pair of revisions (stored in revisions.txt)"""
    PRINT("REVISION LOGS")
    
    klee_out_regex = r"[a-zA-Z0-9/\\_.\s]*(klee-out-)\d+"

    for root, subdirs, files in os.walk(path):
        PRINT("Root: '{}'".format(root))
        if re.match(klee_out_regex, root): # Ignore klee output dir
            continue
        for f in files:
            PRINT("\tFile: '{}'".format(f))
            if f == REV_FILE:
                return os.path.join(root, f)

    print("Error: Revision file '{}' on path '{}' not found".format(REV_FILE, path), file=sys.stderr)

def getRevisionLogs(paths):
    return [getRevisionLog(p) for p in paths]

def getKleeOutDirs(paths):
    """Returns absolute path of klee-out-* dirs"""
    paths = [getAbsPath(p) for p in paths]
    klee_out_dirs = []
    pattern = r"[a-zA-Z0-9/\\_.\s]*(klee-out-)\d+"

    for path in paths:
        PRINT("Path: '{}'".format(path))

        if os.path.isdir(path) and re.match(pattern, path):
            PRINT("\tValid Klee Output Directory")
            klee_out_dirs.append(path)
        else:
            PRINT("\nUsing os.walk(...)...\n") 

            for root, subdirs, files in os.walk(path):
                PRINT ("Root: '{}'".format(root))
                PRINT ("Subdirs: ")

                for d in subdirs:
                    PRINT("\t'{}'".format(d))
                    if os.path.isdir(os.path.join(root, d)) and re.match(pattern, d):
                        PRINT("\tValid Klee Output Directory")
                        klee_out_dirs.append(os.path.join(root, d))

    return klee_out_dirs

def getLogFiles(paths):
    """Get multiple 'run.stats' files from every KLEE run."""
    logs = []

    for dir in getKleeOutDirs(paths):
        for fname in os.listdir(dir):
            if fname == STATS_FILE:
                logs.append(os.path.join(dir, STATS_FILE))
                # Should only be one stats file
                break
    return logs

class LazyEvalList:
    """Store all the lines in run.stats and eval() when needed."""
    def __init__(self, lines):
        # The first line in 'run.stats' contains
        self.lines = lines[1:]

    def __getitem__(self, index):
        if isinstance(self.lines[index], str):
            self.lines[index] = eval(self.lines[index])
        return self.lines[index]

    def __len__(self):
        return len(self.lines)

def stripCommonPathPrefix(paths):
    paths = map(os.path.normpath, paths)
    paths = [p.split('/') for p in paths]
    zipped = zip(*paths)
    i = 0
    for i, elts in enumerate(zipped):
        if len(set(elts)) > 1:
                break
        return ['/'.join(p[i:]) for p in paths]

def getKeyIndex(key, labels):
    """Get the index of the specified key in the labels"""
    def normalizeKey(key):
        return re.split('\W', key)[0]

    for i, title in enumerate(labels):
        if normalizeKey(title) == normalizeKey(key):
            return i
        else:
            raise ValueError('invalid key: {0}'.format(key))

def getLabels():
    labels = ('Instrs', 'Time(s)', 'TUser(%)', 'Tests', 'ICov(%)',
              'BCov(%)', 'Function', 'Category', 'Revisions')
    return labels

def getRow(record, stats, pr):
    """Compose data for the current run into a row."""
    return

def getArguments():
    """Grab user supplied arguments"""
    parser = argparse.ArgumentParser(
            description='output statistics gathered from KLEE run',
            epilog='LEGEND\n',
            formatter_class=argparse.RawDescriptionHelpFormatter
            )
    
    parser.add_argument('dirs', nargs='+', help='klee output directories')

    return parser.parse_args()

def main():
    def validateInt(value):
        """Function for validating integers"""
        try:
            value = int(value)
        except ValueError:
            raise argparse.ArgumentTypeError(
                    'integer expected: {0}'.format(value))
        if value <= 0:
            raise argparse.ArgumentTypeError(
                    'positive integer expected: {0}'.format(value))
        return value


    args = getArguments()

    klee_out_dirs = getKleeOutDirs(args.dirs)
    if len(klee_out_dirs) == 0:
        print('no klee output dir found', file=sys.stderr)
        exit(1)

    revision_logs = getRevisionLogs(args.dirs)
    PRINT("Revision Logs: '{}'".format(revision_logs))

    # Read contents from every 'run.stats' file
    data = [LazyEvalList(list(open(logfile))) for logfile in getLogFiles(klee_out_dirs)]
    if len(data) > 1:
        klee_out_dirs = stripCommonPathPrefix(klee_out_dirs)
    
    # Attach the stripped path
    data = list(zip(klee_out_dirs, data))

    labels = getLabels()

if __name__ == "__main__":
    main()
