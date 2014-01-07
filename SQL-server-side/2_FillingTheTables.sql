USE master
GO

USE SportInfrastructure
GO

INSERT INTO Sport
VALUES
	('Football'),
	('Basketball'),
	('Volleyball'),
	('Athletics'),
	('Gymnastics'),
	('Karate'),
	('Aikido'),
	('Wushu'),
	('Boxing'),
	('Kick-Boxing')
GO

INSERT INTO Organization
VALUES
	('Some Sport Organization', 'Street number 1'),
	('Martial Arts', 'Street number 2'),
	('International Sports', 'Street number 3')
GO

INSERT INTO Person
VALUES
	('Jack',		'Newman',	  'A.',  '1984-03-05'),
	('Matt',		'James',	  'B.',  '1984-04-20'),
	('Sally',		'Fraser',     'C.',  '1985-12-18'),
	('Oliver',		'Gray',		  'D.',  '1986-02-25'),
	('Olivia',		'White',	  'E.',  '1986-02-28'),
	('Ian',			'Black',	  'F.',  '1986-03-12'),
	('Carl',		'McLean',	  'G.',  '1986-09-30'),
	('Joseph',		'Paterson',   'H.',  '1987-01-09'),
	('Keith',		'Manning',    'I.',  '1988-06-13'),
	('Stephanie',	'Grant',      'J.',  '1988-07-22'),
	('Molly',		'Russell',    'K.',  '1988-09-23'),
	('Bella',		'Cameron',    'L.',  '1989-03-21'),
	('Julian',		'Parr',       'M.',  '1989-07-21'),
	('Anthony',		'Tucker',     'N.',  '1989-07-25'),
	('Boris',		'Randall',    'O.',  '1989-08-21'),
	('Alan',		'Rutherford', 'P.',  '1989-11-23'),
	('Jan',			'Baker',	  'Q.',  '1990-04-20'),
	('Bella',		'White',	  'R.',  '1991-04-18'),
	('Jasmine',		'Slater',	  'S.',  '1991-05-02'),
	('Jake',		'Vance',	  'T,',  '1991-05-29'),
	('Sally',		'Allan',	  'U.',  '1991-07-01'),
	('Luke',		'Lawrence',   'V.',  '1991-10-18'),
	('Colin',		'Murray',	  'W.',  '1992-05-26'),
	('Robert',		'Young',	  'X.',  '1992-07-20'),
	('Caroline',	'Grant',	  'Y.',  '1992-08-18'),
	('Ryan',		'Cameron',	  'Z.',  '1993-02-09'),
	('Diana',		'Mathis',	  'A.',  '1993-02-26'),
	('Dorothy',		'Graham',	  'B.',  '1994-08-16'),
	('Audrey',		'Wallace',	  'C.',  '1996-04-04'),
	('Jane',		'Mackenzie',  'D.',  '1996-07-05')
GO

INSERT INTO Building
VALUES
	(0, 'Football Stadium', 'Street number 5', 'Stadium', 400, 250),
	(0, 'Basketball and Volleyball Hall', 'Street number 1', 'Hall', 250, 200),
	(1, 'Competition Hall', 'Street number 2', 'Hall', 350, 300),
	(2, 'Big Sports Hall', 'Street number 3', 'Hall', 300, 300),
	(2, 'International Arena', 'Street number 5', 'Arena', 320, 280),
	(1, 'Dojo', 'Street number 6', 'Training Hall', 100, 150),
	(2, 'Training Hall', 'Street number 6', 'Training Hall', 60, 80)
GO

INSERT INTO Club
VALUES
	(0, 'Football club'),
	(1, 'Iaido club'),
	(1, 'Box && Kick-boxing club'),
	(2, 'Different sports club'),
	(1, 'Martial Arts club')
GO

INSERT INTO Experience
VALUES
	(0, 0, 'Master'),
    (1, 1, 'Master'),
    (2, 2, 'Master'),
    (3, 3, 'Master'),
    (4, 4, 'Master'),
    (5, 5, 'Master'),
    (6, 6, 'Master'),
    (7, 7, 'Master'),
    (8, 8, 'Master'),
    (9, 9, 'Master'),
    (10, 0, 'Intermediate'),
    (11, 1, 'Intermediate'),
    (12, 2, 'Intermediate'),
    (13, 3, 'Intermediate'),
    (14, 4, 'Intermediate'),
    (15, 5, 'Intermediate'),
    (17, 6, 'Intermediate'),
    (18, 7, 'Intermediate'),
    (19, 8, 'Intermediate'),
    (20, 9, 'Intermediate'),
    (21, 0, 'Beginner' ),
    (22, 1, 'Beginner' ),
    (23, 2, 'Beginner' ),
    (24, 3, 'Beginner' ),
    (25, 4, 'Beginner' ),
    (26, 5, 'Beginner' ),
    (27, 6, 'Beginner' ),
    (28, 7, 'Beginner' ),
    (29, 8, 'Beginner' )
GO

INSERT INTO Learner
VALUES
	(10, 0, 0, 0),
    (11, 1, 1, 3),
    (12, 2, 2, 3),
    (13, 3, 3, 3),
    (14, 4, 4, 3),
    (15, 5, 5, 4),
    (17, 6, 6, 1),
    (18, 7, 7, 4),
    (19, 8, 8, 2),
    (20, 9, 9, 2),
    (21, 0, 0, 0),
    (22, 1, 1, 3),
    (23, 2, 2, 3),
    (24, 3, 3, 3),
    (25, 4, 4, 3),
    (26, 5, 5, 4),
    (27, 6, 6, 1),
    (28, 7, 7, 4),
    (29, 8, 8, 2)
GO

INSERT INTO Competition
VALUES
	(0, 0, 'Competition #1', '2010-12-12'),
	(1, 1, 'Competition #2', '2011-12-12'),
	(2, 3, 'Competition #2', '2012-12-12'),
	(3, 4, 'Competition #4', '2013-12-12'),
	(4, 8, 'Competition #5', '2010-05-05'),
	(5, 5, 'Competition #6', '2011-06-07'),
	(6, 7, 'Competition #7', '2012-10-10')
GO

INSERT INTO Participant
VALUES
	(0, 0, 0, 50)
GO