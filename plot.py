import matplotlib.pyplot as plt


if __name__ == "__main__":
    with open("data.txt","r") as f: data = f.read().split("\n")
    X,Y = [], []
    for d in data:
        if d:
            point = d.split(",")
            X.append(int(point[0]))
            Y.append(int(point[1]))

    # Plot number of pages per window over time
    plt.figure(0)
    plt.plot(X,Y)
    plt.title("Number of pages per window over time")
    plt.xlabel("Window")
    plt.ylabel("Number of pages")
    plt.savefig("time.png")

    # Plot frequency of number of pages
    plt.figure(1)
    plt.hist(Y)
    plt.title("Frequency of number of pages per window")
    plt.xlabel("Number of pages per window")
    plt.ylabel("Frequency")
    plt.savefig("hist.png")