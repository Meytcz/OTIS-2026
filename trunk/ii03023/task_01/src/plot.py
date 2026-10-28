import csv
from collections import defaultdict
import matplotlib.pyplot as plt

data = defaultdict(lambda: ([], []))
with open("results.csv", encoding="utf-8") as f:
    for row in csv.DictReader(f):
        xs, ys = data[(row["model"], row["input"])]
        xs.append(int(row["tau"]))
        ys.append(float(row["y"]))

models = sorted({key[0] for key in data})

# Отдельный график для каждой модели
for model in models:
    fig, ax = plt.subplots(figsize=(9, 4.5))
    for (m, inp), (xs, ys) in data.items():
        if m == model:
            ax.plot(xs, ys, marker=".", label=inp)
    ax.set_title(model)
    ax.set_xlabel("tau")
    ax.set_ylabel("y")
    ax.grid(True)
    ax.legend()
    fig.tight_layout()
    file_name = "model_" + model.split()[1].replace(".", "_") + ".png"
    fig.savefig(file_name, dpi=120)
    plt.close(fig)
    print("Plot saved to", file_name)

# Общий график: все модели друг под другом
fig, axes = plt.subplots(len(models), 1, figsize=(9, 3.2 * len(models)))
for ax, model in zip(axes, models):
    for (m, inp), (xs, ys) in data.items():
        if m == model:
            ax.plot(xs, ys, marker=".", label=inp)
    ax.set_title(model)
    ax.set_xlabel("tau")
    ax.set_ylabel("y")
    ax.grid(True)
    ax.legend()

fig.tight_layout()
fig.savefig("plot.png", dpi=120)
print("Plot saved to plot.png")
plt.show()
