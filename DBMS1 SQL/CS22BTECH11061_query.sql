--1.a
SELECT instructor.id AS instructor_id, instructor.name AS instructor_name
FROM instructor
INNER JOIN teaches ON teaches.id = instructor.id
GROUP BY instructor.id
ORDER BY COUNT(DISTINCT teaches.course_id) DESC
LIMIT 3;


--1.b
SELECT instructor.id AS instructor_id, instructor.name AS instructor_name
FROM instructor
INNER JOIN teaches ON teaches.id = instructor.id 
WHERE instructor.dept_name = 'Statistics'
GROUP BY instructor.id
ORDER BY COUNT(DISTINCT teaches.course_id) DESC
LIMIT 3;


--2
SELECT instructor.dept_name, teaches.course_id, course.title, section.sec_id, teaches.semester, teaches.year, total_enrollments
FROM instructor 
LEFT JOIN teaches ON teaches.id = instructor.id
LEFT JOIN course ON course.course_id = teaches.course_id
LEFT JOIN
    (SELECT course_id AS c_id, COUNT(id) AS total_enrollments
    FROM takes 
    GROUP BY course_id)
ON c_id = course.course_id
LEFT JOIN section ON section.course_id = course.course_id
WHERE instructor.id = (SELECT id FROM instructor ORDER BY salary DESC LIMIT 1)
ORDER BY teaches.course_id, teaches.year, teaches.semester;


--3
SELECT DISTINCT course.course_id, course.title, course.dept_name, instructor.name AS instructor_name, registered, teaches.sec_id, teaches.semester, teaches.year, section.time_slot_id
FROM course
LEFT JOIN teaches ON course.course_id = teaches.course_id
LEFT JOIN 
    (SELECT course_id, year, semester, COUNT(id) AS registered
    FROM takes
    GROUP BY course_id, year, semester) 
    AS take
ON (take.course_id = course.course_id and take.semester = teaches.semester and take.year = teaches.year)
LEFT JOIN instructor ON instructor.id = teaches.id
LEFT JOIN section ON (course.course_id = section.course_id and teaches.sec_id = section.sec_id and teaches.semester = section.semester and teaches.year = section.year) 
WHERE course.course_id = '362'
ORDER BY teaches.year DESC;


--4
SELECT COUNT(takes.id) AS out_of_department_student_registrations
FROM takes
LEFT JOIN student ON student.id = takes.id
LEFT JOIN course ON course.course_id = takes.course_id
WHERE course.course_id = '319' and year = 2003 and student.dept_name != course.dept_name;


--5
SELECT student.id, name, student.dept_name, SUM(credits) AS total_credits
FROM student
LEFT JOIN takes ON takes.id = student.id
LEFT JOIN course ON course.course_id = takes.course_id
GROUP BY student.id
ORDER BY total_credits DESC, name 
LIMIT 3;


--6
SELECT DISTINCT course_id, title
FROM course
WHERE course_id NOT IN
    (SELECT DISTINCT course_id
    FROM teaches
    WHERE year = 2003 OR year = 2004)
ORDER BY course_id;


--7
SELECT course.course_id AS course__id, course.title, instructor.name AS instructor_name, teaches.year
FROM course
INNER JOIN teaches ON teaches.course_id = course.course_id
LEFT JOIN instructor ON instructor.id = teaches.id
WHERE teaches.year IN
        (SELECT MAX(year)
        FROM teaches
        GROUP BY course_id
        HAVING COUNT(DISTINCT year) = 1
        ORDER BY MAX(year) DESC
        LIMIT 1)
    AND course.course_id IN
        (SELECT teaches.course_id
        FROM teaches
        GROUP BY course_id
        HAVING COUNT(DISTINCT teaches.year) = 1)
ORDER BY course.course_id;


--8
SELECT course_id, title 
FROM course
WHERE LENGTH(title) > 15 AND title ILIKE '%sys%'
ORDER BY course_id;


--9
SELECT dept_name, avg(salary) AS avg_salary
FROM instructor
GROUP BY dept_name
ORDER BY avg_salary DESC
LIMIT 1;


--10
SELECT instructor.id, instructor.name, instructor.dept_name
FROM instructor
LEFT JOIN
    (SELECT teaches.id AS teaches_id, COUNT(course_id) AS taught
    FROM teaches
    WHERE year = 2003
    GROUP BY teaches.id)
ON teaches_id = instructor.id
WHERE taught = 1 OR taught IS NULL
ORDER BY instructor.id;