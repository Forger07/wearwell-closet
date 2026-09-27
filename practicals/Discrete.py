
print("Q1")
A = {10, 20, 30, 40, 50}
print("Set A:", A)

print("\nQ2")
A = {10, 20, 20, 30, 30, 40}
print("Set A:", A)

print("\nQ3")
A = {1, 2, 3, 4, 5}
B = {4, 5, 6, 7, 8}
print("Set A:", A)
print("Set B:", B)

print("\nQ4")
print(A.union(B))
print(A | B)

print("\nQ5")
print(A.intersection(B))
print(A & B)

print("\nQ6")
print("A - B:", A.difference(B))
print("B - A:", B.difference(A))

print("\nQ7")
print(A.symmetric_difference(B))
print(A ^ B)

print("\nQ8")
print(5 in A)
print(10 not in A)

print("\nQ9")
A = {10, 20, 30, 40, 50}
print("Number of elements:", len(A))

print("\nQ10")
A = {10, 20, 30, 40, 50}
A.add(60)
print(A)

print("\nQ11")
A = {10, 20, 30, 40, 50}
A.update([60, 70, 80])
print(A)

print("\nQ12")
A = {10, 20, 30, 40, 50}
A.remove(30)
print(A)

print("\nQ13")
A = {10, 20, 30, 40, 50}
A.discard(30)
A.discard(100)   # no error even though 100 isn't in A
print(A)

print("\nQ14")
A = {1, 2, 3}
B = {1, 2, 3, 4, 5}
print(A.issubset(B))
print(A <= B)

print("\nQ15")
print(B.issuperset(A))
print(B >= A)

print("\nQ16")
A = {1, 2, 3}
B = {10, 20, 30}
print(A.isdisjoint(B))

print("\nQ17")
A = {1, 2, 3}
B = {3, 4, 5}
print(A.isdisjoint(B))

print("\nQ18")
A = {1, 2, 3}
B = {3, 2, 1}
print(A == B)

print("\nQ19")
python_students = {"Amit", "Riya", "Neha", "Rahul"}
r_students = {"Riya", "Rahul", "Sneha", "Karan"}
print(python_students.intersection(r_students))

print("\nQ20")
print("Either:", python_students.union(r_students))
print("Only Python:", python_students.difference(r_students))
print("Exactly one:", python_students.symmetric_difference(r_students))

print("\nMini Assignment")
course_A = {"Amit", "Riya", "Neha", "Rahul", "Pooja"}
course_B = {"Riya", "Rahul", "Sneha", "Karan", "Pooja"}

print("Either course:", course_A.union(course_B))
print("Both courses:", course_A.intersection(course_B))
print("Only Course A:", course_A.difference(course_B))
print("Only Course B:", course_B.difference(course_A))
print("Exactly one:", course_A.symmetric_difference(course_B))
print("Total unique students:", len(course_A.union(course_B)))
