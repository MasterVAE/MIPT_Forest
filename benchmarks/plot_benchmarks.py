from pathlib import Path

import numpy as np
import matplotlib.pyplot as plt


# ============================================================
# Настройки
# ============================================================

DATA_DIR = Path("benchmarks/data")
PLOTS_DIR = Path("benchmarks/plots")

STRUCTURES = [
    ("set", "std::set"),
    ("vec", "std::vector"),
    ("nat", "BST"),
    ("avl", "AVL"),
]

DISTRIBUTIONS = [
    ("random", "Random"),
    ("ascending", "Ascending"),
    ("descending", "Descending"),
    ("clustered", "Clustered"),
]

BENCHMARKS = {
    "insert": "Insertion",
    "smaller": "n x — Count elements < x",
    "kth": "m i — Find i-th element",
    "range": "q l r — Range query",
    "mixed": "Mixed workload",
}


# ============================================================
# Загрузка данных
# ============================================================

def load_benchmark(structure: str, benchmark: str):
    path = DATA_DIR / structure / f"{benchmark}.txt"

    if not path.exists():
        print(f"WARNING: file not found: {path}")
        return None

    data = np.loadtxt(path, comments="#")

    if data.ndim == 1:
        data = data.reshape(1, -1)

    return data


# ============================================================
# Один график benchmark-а
# ============================================================

def plot_benchmark(benchmark: str, title: str):
    plt.figure(figsize=(11, 7))

    has_data = False

    for distribution_index, (distribution, distribution_name) in enumerate(
        DISTRIBUTIONS
    ):
        for structure, structure_name in STRUCTURES:
            data = load_benchmark(structure, benchmark)

            if data is None:
                continue

            if data.shape[1] <= distribution_index + 1:
                continue

            x = data[:, 0]
            y = data[:, distribution_index + 1]

            plt.plot(
                x,
                y,
                marker="o",
                label=f"{structure_name} — {distribution_name}",
            )

            has_data = True

    if not has_data:
        print(f"No data for benchmark: {benchmark}")
        plt.close()
        return

    plt.xscale("log")
    plt.yscale("log")

    plt.xlabel("Number of elements N")
    plt.ylabel("Throughput (operations/sec)")
    plt.title(title)

    plt.grid(True, which="both", alpha=0.3)
    plt.legend(fontsize=9)

    plt.tight_layout()

    output = PLOTS_DIR / f"{benchmark}.png"
    plt.savefig(output, dpi=200)
    plt.close()

    print(f"Saved: {output}")


# ============================================================
# Отдельный график для каждого распределения
# ============================================================

def plot_benchmark_by_distribution(benchmark: str, title: str):
    for distribution_index, (distribution, distribution_name) in enumerate(
        DISTRIBUTIONS
    ):
        plt.figure(figsize=(9, 6))

        has_data = False

        for structure, structure_name in STRUCTURES:
            data = load_benchmark(structure, benchmark)

            if data is None:
                continue

            if data.shape[1] <= distribution_index + 1:
                continue

            x = data[:, 0]
            y = data[:, distribution_index + 1]

            plt.plot(
                x,
                y,
                marker="o",
                label=structure_name,
            )

            has_data = True

        if not has_data:
            plt.close()
            continue

        plt.xscale("log")
        plt.yscale("log")

        plt.xlabel("Number of elements N")
        plt.ylabel("Throughput (operations/sec)")
        plt.title(f"{title} — {distribution_name}")

        plt.grid(True, which="both", alpha=0.3)
        plt.legend()

        plt.tight_layout()

        output = (
            PLOTS_DIR
            / f"{benchmark}_{distribution}.png"
        )

        plt.savefig(output, dpi=200)
        plt.close()

        print(f"Saved: {output}")


# ============================================================
# Главная функция
# ============================================================

def main():
    PLOTS_DIR.mkdir(parents=True, exist_ok=True)

    for benchmark, title in BENCHMARKS.items():
        print(f"\n=== {title} ===")

        # Один общий график
        plot_benchmark(benchmark, title)

        # И отдельный график для каждого распределения
        plot_benchmark_by_distribution(
            benchmark,
            title,
        )

    print("\nDone.")


if __name__ == "__main__":
    main()