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
    _fields_ = [("time_seq", ctypes.c_float),
                ("time_l", ctypes.c_float),
                ("time_lfree", ctypes.c_float) ]

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
            tmp = [[],[],[]]
            for r in range(0, self.repetitions_per_point):
                result = self.bench_function( x, *self.parameters )
                tmp[0].append(result.time_seq*1000)
                tmp[1].append(result.time_l*1000)
                tmp[2].append(result.time_lfree*1000)
            self.data_seq[x] = tmp[0]
            self.data_l[x] = tmp[1]
            self.data_lfree[x] = tmp[2]

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
        with open(f"{self.basedir}/data/{self.now}/avg/{self.name}_seq.data", "w")\
                as datafile:
            datafile.write(f"x datapoint\n")
            for x, box in self.data_seq.items():
                datafile.write(f"{x} {sum(box)/len(box)}\n")
            print("Data written to: " + datafile.name)
        with open(f"{self.basedir}/data/{self.now}/avg/{self.name}_l.data", "w")\
                as datafile:
            datafile.write(f"x datapoint\n")
            for x, box in self.data_l.items():
                datafile.write(f"{x} {sum(box)/len(box)}\n")
            print("Data written to: " + datafile.name)
        with open(f"{self.basedir}/data/{self.now}/avg/{self.name}_lfree.data", "w")\
                as datafile:
            datafile.write(f"x datapoint\n")
            for x, box in self.data_lfree.items():
                datafile.write(f"{x} {sum(box)/len(box)}\n")
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
    # smallbench_100 = Benchmark(binary.small_bench, (100,3), 3,
    #                            num_threads, basedir, "smallbench_100")
    smallbench_10000_3 = Benchmark(binary.small_bench, (10000,3), 3,
                               num_threads, basedir, "smallbench_10000_3", curr_time)
    smallbench_10000_5 = Benchmark(binary.small_bench, (10000,5), 3,
                               num_threads, basedir, "smallbench_10000_5", curr_time)
    smallbench_10000_10 = Benchmark(binary.small_bench, (10000,10), 3,
                               num_threads, basedir, "smallbench_10000_10", curr_time)

    # smallbench_100.run()
    # smallbench_100.write_avg_data()
    smallbench_10000_3.run()
    smallbench_10000_3.write_avg_data()
    smallbench_10000_5.run()
    smallbench_10000_5.write_avg_data()
    smallbench_10000_10.run()
    smallbench_10000_10.write_avg_data()

if __name__ == "__main__":
    benchmark()
