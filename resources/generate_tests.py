import argparse
import random
from bisect import bisect_left, bisect_right


# ============================================================
# Генерация случайных команд
# ============================================================

def generate_command(rng, values):
    """
    Генерирует одну логическую команду и одновременно
    возвращает правильный ответ, если команда является запросом.
    """

    command_type = rng.choices(
        ["k", "q", "n"],
        weights=[45, 35, 20],
        k=1
    )[0]

    # --------------------------------------------------------
    # k x
    # --------------------------------------------------------
    if command_type == "k":
        x = rng.randint(-1_000_000, 1_000_000)

        # Множество уникальных ключей.
        if x not in values:
            values.append(x)
            values.sort()

        return f"k {x}", None

    # --------------------------------------------------------
    # q l r
    #
    # Количество элементов в интервале (l, r]
    # --------------------------------------------------------
    elif command_type == "q":
        l = rng.randint(-1_000_000, 1_000_000)
        r = rng.randint(-1_000_000, 1_000_000)

        if l > r:
            answer = 0
        else:
            left = bisect_right(values, l)
            right = bisect_right(values, r)
            answer = right - left

        return f"q {l} {r}", answer

    # --------------------------------------------------------
    # n x
    #
    # Количество ключей строго меньше x
    # --------------------------------------------------------
    else:
        x = rng.randint(-1_000_000, 1_000_000)

        answer = bisect_left(values, x)

        return f"n {x}", answer


# ============================================================
# Генерация одного теста
# ============================================================

def generate_test(rng, commands_per_test, values):
    """
    Генерирует один тест с заданным количеством
    логических команд.
    """

    commands = []
    answers = []

    for _ in range(commands_per_test):
        command, answer = generate_command(rng, values)

        commands.append(command)

        if answer is not None:
            answers.append(str(answer))

    return " ".join(commands), " ".join(answers)


# ============================================================
# Основная функция
# ============================================================

def generate_tests(test_count, commands_per_test, output_file, seed):
    rng = random.Random(seed)

    # Важно:
    # Tree в твоём тестере не пересоздаётся между тестами,
    # поэтому состояние дерева сохраняем между тестами.
    values = []

    total_commands = test_count * commands_per_test

    print("Генерация тестов...")
    print(f"Количество тестов:       {test_count}")
    print(f"Команд в одном тесте:    {commands_per_test}")
    print(f"Всего команд:            {total_commands}")
    print(f"Seed:                     {seed}")
    print(f"Файл:                     {output_file}")

    with open(output_file, "w", encoding="utf-8") as file:
        file.write(f"{test_count}\n")

        for test_number in range(test_count):
            commands, answers = generate_test(
                rng,
                commands_per_test,
                values
            )

            file.write(commands + "\n")
            file.write(answers + "\n")

            if (test_number + 1) % max(1, test_count // 10) == 0:
                progress = (test_number + 1) * 100 // test_count
                print(f"Прогресс: {progress}%")

    print()
    print("Готово!")
    print(f"Всего логических команд: {total_commands}")


# ============================================================
# Парсинг аргументов
# ============================================================

def main():
    parser = argparse.ArgumentParser(
        description="Генератор больших тестов для MIPT_Forest"
    )

    parser.add_argument(
        "--tests",
        type=int,
        default=1000,
        help="Количество тестов (по умолчанию: 1000)"
    )

    parser.add_argument(
        "--commands",
        type=int,
        default=1000,
        help="Количество логических команд в каждом тесте (по умолчанию: 1000)"
    )

    parser.add_argument(
        "--output",
        type=str,
        default="resources/test.tst",
        help="Имя выходного файла (по умолчанию: test.tst)"
    )

    parser.add_argument(
        "--seed",
        type=int,
        default=0x5EED1234,
        help="Seed генератора случайных чисел"
    )

    args = parser.parse_args()

    if args.tests <= 0:
        raise ValueError("Количество тестов должно быть больше 0")

    if args.commands <= 0:
        raise ValueError(
            "Количество команд в тесте должно быть больше 0"
        )

    generate_tests(
        test_count=args.tests,
        commands_per_test=args.commands,
        output_file=args.output,
        seed=args.seed
    )


if __name__ == "__main__":
    main()