use master
go

use SportInfrastructure
go

CREATE ROLE adminRole
go

CREATE LOGIN rootLogin WITH PASSWORD='root_password123456'
go

CREATE USER rootUser FROM LOGIN rootLogin
go

GRANT EXECUTE ON BuildlingByType TO adminRole WITH GRANT OPTION
GRANT EXECUTE ON BuildlingWithPlaces TO adminRole WITH GRANT OPTION
GRANT EXECUTE ON CoachesOfSportsman TO adminRole WITH GRANT OPTION
GRANT EXECUTE ON CompetitionsBetweenDate TO adminRole WITH GRANT OPTION
GRANT EXECUTE ON CompetitionsByOrganization TO adminRole WITH GRANT OPTION
GRANT EXECUTE ON SportsmenOfCoach TO adminRole WITH GRANT OPTION
GRANT EXECUTE ON CompetitionWinners TO adminRole WITH GRANT OPTION
GRANT EXECUTE ON ThoseWhoDidntTakePartInCompetitions TO adminRole WITH GRANT OPTION
GRANT EXECUTE ON CoachesSport TO adminRole WITH GRANT OPTION
GRANT EXECUTE ON CompetitionsSport TO adminRole WITH GRANT OPTION
GRANT EXECUTE ON ThoseWhoStudyMoreThanOneSport TO adminRole WITH GRANT OPTION
GRANT EXECUTE ON CompetitionsInBuildling TO adminRole WITH GRANT OPTION

EXEC  sp_addrolemember 'adminRole', 'rootUser'
go

CREATE ROLE userRole
go

CREATE LOGIN userLogin WITH PASSWORD='User12345678' /*, DEFAULT_DATABASE = name*/
go

CREATE USER simpleUser FROM LOGIN userLogin
go

GRANT SELECT ON AllCompetitions TO userRole WITH GRANT OPTION  
GRANT SELECT ON AllParticipants TO userRole WITH GRANT OPTION 
GRANT SELECT ON SportsmenWithCoaches TO userRole WITH GRANT OPTION 
GRANT SELECT ON SportsmenWithSports TO userRole WITH GRANT OPTION 

EXEC  sp_addrolemember 'userRole', 'simpleUser'
go

DROP LOGIN userLogin
DROP USER simpleUser
DROP ROLE userRole

use master
go

use SportInfrastructure
go

select * from Club
go


/* Create a new role for executing stored
   procedures */
CREATE ROLE db_executor

 
/* Grant stored procedure execute rights
   to the role */
GRANT EXECUTE TO db_executor

 
/* Add a user to the db_executor role */
ALTER ROLE db_executor ADD MEMBER User
