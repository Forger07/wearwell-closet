print("Press n or N to exit the program.")

while True:
    user_val = input("Enter marks: ")

    if user_val.lower() == 'n':
        break

    try:
        mark = float(user_val)
    except ValueError:
        print("Invalid input. Please enter a number between 0 and 100.")
        continue

    if mark < 0 or mark > 100:
        print("Invalid input. Please enter a value between 0 and 100.")
    elif mark >= 80:
        print("Grade O")
    elif mark >= 72:
        print("Grade A+")
    elif mark >= 66:
        print("Grade A")
    elif mark >= 54:
        print("Grade B+")
    elif mark >= 50:
        print("Grade B")
    else:
        print("Grade F")
