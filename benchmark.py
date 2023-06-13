import ctypes
import os
import datetime
import time

# TODO: define performance counters
# class cBenchCounters(ctypes.Structure):
#     '''
#     This has to match the returned struct in library.c
#     '''
#     _fields_ = [ ("failed_turns", ctypes.c_int),
#                  ("successful_lends", ctypes.c_int) ]

class cBenchResult(ctypes.Structure):
    '''
    This has to match the returned struct in library.c
    '''
    _fields_ = [("time_seq", ctypes.c_double),
                ("time_l", ctypes.c_double),
                ("time_lfree", ctypes.c_double),
                ("time_lfree_improved", ctypes.c_double),
                ("time_lfree_pred", ctypes.c_double),
                ("throughput_seq", ctypes.c_double),
                ("throughput_l", ctypes.c_double),
                ("throughput_lfree", ctypes.c_double),
                ("throughput_lfree_improved", ctypes.c_double),
                ("throughput_lfree_pred", ctypes.c_double) ]

class Benchmark:
    '''
    Class representing a benchmark. It assumes any benchmark sweeps over some
    parameter xrange using the fixed set of inputs for every point. It provides
    two ways of averaging over the given amount of repetitions:
    - represent everything in a boxplot, or
    - average over the results.
    '''
    def __init__(self, bench_function, parameters,
                 repetitions_per_point, xrange, basedir, name, time=None):
        self.bench_function = bench_function
        self.parameters = parameters
        self.repetitions_per_point = repetitions_per_point
        self.xrange = xrange
        self.basedir = basedir
        self.name = name
        self.now = time

        self.data_seq = {}
        self.data_l = {}
        self.data_lfree = {}
        self.data_lfree_improved = {}
        self.data_lfree_pred = {}

    def run(self):
        '''
        Runs the benchmark with the given parameters. Collects
        repetitions_per_point data points and writes them back to the data
        dictionary to be processed later.
        '''
        if self.now == None:
            self.now = datetime.datetime.now().strftime("%Y-%m-%dT%H:%M:%S")
        print(f"Starting Benchmark run at {self.now}")

        for x in self.xrange:
            tmp = [[],[],[],[],[]]
            for r in range(0, self.repetitions_per_point):
                result = self.bench_function( x, *self.parameters )
                tmp[0].append((result.time_seq*1000, result.throughput_seq))
                tmp[1].append((result.time_l*1000, result.throughput_l))
                tmp[2].append((result.time_lfree*1000, result.throughput_lfree))
                tmp[3].append((result.time_lfree_improved*1000, result.throughput_lfree_improved))
                tmp[4].append((result.time_lfree_pred*1000, result.throughput_lfree_pred))
            self.data_seq[x] = tmp[0]
            self.data_l[x] = tmp[1]
            self.data_lfree[x] = tmp[2]
            self.data_lfree_improved[x] = tmp[3]
            self.data_lfree_pred[x] = tmp[4]

    def write_avg_data(self):
        '''
        Writes averages for each point measured into a dataset in the data
        folder timestamped when the run was started.
        '''
        if self.now is None:
            raise Exception("Benchmark was not run. Run before writing data.")

        try:
            os.makedirs(f"{self.basedir}/data/{self.now}/avg")
        except FileExistsError:
            pass
        try:
            os.makedirs(f"{self.basedir}/data/{self.now}/throughput")
        except FileExistsError:
            pass
        with open(f"{self.basedir}/data/{self.now}/avg/{self.name}_seq.data", "w")\
                as datafile:
            datafile.write(f"x datapoint\n")
            for x, box in self.data_seq.items():
                datafile.write(f"{x} {sum([data[0] for data in box])/len(box)}\n")
            print("Data written to: " + datafile.name)
        with open(f"{self.basedir}/data/{self.now}/throughput/{self.name}_seq.data", "w")\
                as datafile:
            datafile.write(f"x datapoint\n")
            for x, box in self.data_seq.items():
                datafile.write(f"{x} {sum([data[1] for data in box])/len(box)}\n")
            print("Data written to: " + datafile.name)

        with open(f"{self.basedir}/data/{self.now}/avg/{self.name}_l.data", "w")\
                as datafile:
            datafile.write(f"x datapoint\n")
            for x, box in self.data_l.items():
                datafile.write(f"{x} {sum([data[0] for data in box])/len(box)}\n")
            print("Data written to: " + datafile.name)
        with open(f"{self.basedir}/data/{self.now}/throughput/{self.name}_l.data", "w")\
                as datafile:
            datafile.write(f"x datapoint\n")
            for x, box in self.data_l.items():
                datafile.write(f"{x} {sum([data[1] for data in box])/len(box)}\n")
            print("Data written to: " + datafile.name)

        with open(f"{self.basedir}/data/{self.now}/avg/{self.name}_lfree.data", "w")\
                as datafile:
            datafile.write(f"x datapoint\n")
            for x, box in self.data_lfree.items():
                datafile.write(f"{x} {sum([data[0] for data in box])/len(box)}\n")
            print("Data written to: " + datafile.name)
        with open(f"{self.basedir}/data/{self.now}/throughput/{self.name}_lfree.data", "w")\
                as datafile:
            datafile.write(f"x datapoint\n")
            for x, box in self.data_lfree.items():
                datafile.write(f"{x} {sum([data[1] for data in box])/len(box)}\n")
            print("Data written to: " + datafile.name)

        with open(f"{self.basedir}/data/{self.now}/avg/{self.name}_lfree_improved.data", "w")\
                as datafile:
            datafile.write(f"x datapoint\n")
            for x, box in self.data_lfree_improved.items():
                datafile.write(f"{x} {sum([data[0] for data in box])/len(box)}\n")
            print("Data written to: " + datafile.name)
        with open(f"{self.basedir}/data/{self.now}/throughput/{self.name}_lfree_improved.data", "w")\
                as datafile:
            datafile.write(f"x datapoint\n")
            for x, box in self.data_lfree_improved.items():
                datafile.write(f"{x} {sum([data[1] for data in box])/len(box)}\n")
            print("Data written to: " + datafile.name)

        with open(f"{self.basedir}/data/{self.now}/avg/{self.name}_lfree_pred.data", "w")\
                as datafile:
            datafile.write(f"x datapoint\n")
            for x, box in self.data_lfree_pred.items():
                datafile.write(f"{x} {sum([data[0] for data in box])/len(box)}\n")
            print("Data written to: " + datafile.name)
        with open(f"{self.basedir}/data/{self.now}/throughput/{self.name}_lfree_pred.data", "w")\
                as datafile:
            datafile.write(f"x datapoint\n")
            for x, box in self.data_lfree_pred.items():
                datafile.write(f"{x} {sum([data[1] for data in box])/len(box)}\n")
            print("Data written to: " + datafile.name)

def benchmark():
    '''
    Requires the binary to also be present as a shared library.
    '''
    basedir = os.path.dirname(os.path.abspath(__file__))
    binary = ctypes.CDLL( f"{basedir}/main.so" )
    # Set the result type for each benchmark function
    binary.small_bench.restype = cBenchResult

    # The number of threads. This is the x-axis in the benchmark, i.e., the
    # parameter that is 'sweeped' over.
    num_threads = [1,2,4,8,16]#,32,64,128,256]

    curr_time = datetime.datetime.now().strftime("%Y-%m-%dT%H:%M:%S") 

    # Parameters for the benchmark are passed in a tuple, here (1000,). To pass
    # just one parameter, we cannot write (1000) because that would not parse
    # as a tuple, instead python understands a trailing comma as a tuple with
    # just one entry.
    operations = 20000
    repetitions = 3
    smallbench_10000_3_10p = Benchmark(binary.small_bench, (operations,3,10,10), repetitions,
                               num_threads, basedir, "smallbench_10000_3_10p", curr_time)
    smallbench_10000_3 = Benchmark(binary.small_bench, (operations,3,25,25), repetitions,
                               num_threads, basedir, "smallbench_10000_3", curr_time)
    smallbench_10000_5 = Benchmark(binary.small_bench, (operations,5,25,25), repetitions,
                               num_threads, basedir, "smallbench_10000_5", curr_time)
    smallbench_10000_10 = Benchmark(binary.small_bench, (operations,10,25,25), repetitions,
                               num_threads, basedir, "smallbench_10000_10", curr_time)

    smallbench_10000_3_10p.run()
    smallbench_10000_3_10p.write_avg_data()
    smallbench_10000_3.run()
    smallbench_10000_3.write_avg_data()
    smallbench_10000_5.run()
    smallbench_10000_5.write_avg_data()
    smallbench_10000_10.run()
    smallbench_10000_10.write_avg_data()

if __name__ == "__main__":
    benchmark()
