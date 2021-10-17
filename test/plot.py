str = '''
jobnum is 1, thread_num is 2
jobnum is 10, thread_num is 3
jobnum is 9, thread_num is 3
jobnum is 8, thread_num is 3
jobnum is 7, thread_num is 3
jobnum is 6, thread_num is 3
jobnum is 15, thread_num is 6
jobnum is 14, thread_num is 6
jobnum is 13, thread_num is 6
jobnum is 12, thread_num is 6
jobnum is 11, thread_num is 6
jobnum is 10, thread_num is 6
jobnum is 9, thread_num is 6
jobnum is 28, thread_num is 9
jobnum is 27, thread_num is 9
jobnum is 26, thread_num is 9
jobnum is 25, thread_num is 9
jobnum is 24, thread_num is 9
jobnum is 33, thread_num is 16
jobnum is 32, thread_num is 16
jobnum is 31, thread_num is 16
jobnum is 30, thread_num is 16
jobnum is 29, thread_num is 16
jobnum is 28, thread_num is 16
jobnum is 27, thread_num is 16
jobnum is 26, thread_num is 16
jobnum is 25, thread_num is 16
jobnum is 24, thread_num is 16
jobnum is 23, thread_num is 16
jobnum is 22, thread_num is 16
jobnum is 21, thread_num is 16
jobnum is 20, thread_num is 16
jobnum is 19, thread_num is 16
jobnum is 28, thread_num is 16
jobnum is 27, thread_num is 16
jobnum is 26, thread_num is 16
jobnum is 25, thread_num is 16
jobnum is 24, thread_num is 16
jobnum is 23, thread_num is 16
jobnum is 22, thread_num is 16
jobnum is 21, thread_num is 16
jobnum is 20, thread_num is 16
jobnum is 19, thread_num is 16
jobnum is 18, thread_num is 16
jobnum is 17, thread_num is 16
jobnum is 16, thread_num is 16
jobnum is 15, thread_num is 15
jobnum is 14, thread_num is 14
jobnum is 13, thread_num is 13
jobnum is 12, thread_num is 12
jobnum is 11, thread_num is 11
jobnum is 10, thread_num is 10
jobnum is 19, thread_num is 11
jobnum is 18, thread_num is 11
jobnum is 27, thread_num is 16
jobnum is 26, thread_num is 16
jobnum is 25, thread_num is 16
jobnum is 24, thread_num is 16
jobnum is 23, thread_num is 16
jobnum is 22, thread_num is 16
jobnum is 21, thread_num is 16
jobnum is 20, thread_num is 16
jobnum is 19, thread_num is 16
jobnum is 18, thread_num is 16
jobnum is 17, thread_num is 16
jobnum is 16, thread_num is 16
jobnum is 15, thread_num is 15
jobnum is 14, thread_num is 14
jobnum is 13, thread_num is 14
jobnum is 22, thread_num is 14
jobnum is 21, thread_num is 14
jobnum is 29, thread_num is 16
jobnum is 28, thread_num is 16
jobnum is 27, thread_num is 16
jobnum is 26, thread_num is 16
jobnum is 25, thread_num is 16
jobnum is 24, thread_num is 16
jobnum is 23, thread_num is 16
jobnum is 22, thread_num is 16
jobnum is 21, thread_num is 16
jobnum is 20, thread_num is 16
jobnum is 19, thread_num is 16
jobnum is 18, thread_num is 16
jobnum is 17, thread_num is 16
jobnum is 16, thread_num is 16
jobnum is 15, thread_num is 15
jobnum is 14, thread_num is 14
jobnum is 13, thread_num is 14
jobnum is 12, thread_num is 13
jobnum is 11, thread_num is 11
jobnum is 10, thread_num is 10
jobnum is 9, thread_num is 9
jobnum is 8, thread_num is 8
jobnum is 7, thread_num is 7
jobnum is 6, thread_num is 6
jobnum is 5, thread_num is 5
jobnum is 4, thread_num is 4
jobnum is 3, thread_num is 3
jobnum is 2, thread_num is 2
jobnum is 1, thread_num is 2
'''

import matplotlib.pyplot as plt
import numpy as np

if __name__ == "__main__":
    jobnum = []
    threadnum = []
    for line in str.split('\n'):
        if line:
            data = line.replace(',', ' ').split()
            jobnum.append(int(data[2]))
            threadnum.append(int(data[-1]))

    t = np.arange(0, len(jobnum), 1)
    fig, ax = plt.subplots()
    ax.plot(t, jobnum, label="job num")
    ax.plot(t, threadnum, label="thread num")
    ax.set(title = "dynamic thread num")
    ax.legend()
    ax.grid()
    fig.savefig("dynamic.png")
    plt.show()