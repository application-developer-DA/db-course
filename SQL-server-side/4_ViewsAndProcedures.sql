use master
go

use SportInfrastructure
go

create view AllCompetitions
as
select 
	Competition.id [CompetitionId],
	Competition.name [CompetitionName],
	Competition.competition_date [CompetitionDate],
	Sport.id [SportId],
	Sport.name [SportName],
	Building.id [BuildingId],
	Building.name [BuildingName],
	Organization.id [OrganizationId],
	Organization.name [OrganizationName]
from 
	Competition
inner join Sport on Competition.sport_id = Sport.id
inner join Building on Building.id = Competition.building_id
inner join Organization on Organization.id = Building.organization_id
go

create view SportsmenWithSports
as
select 
	Person.id [PersonId],
	Person.firstname [Firstname], 
	Person.lastname [Lastname],
	Person.middlename [Middlename],
	Person.birthdate [Birthdate],
	Sport.id [SportId],
	Sport.name [Sport],
	Experience.id [ExperienceId],
	Experience.title [Title]
from
	Person 
inner join Experience on Experience.person_id = Person.id
inner join Sport on Sport.id = Experience.sport_id
go

create view SportsmenWithCoaches
as
select
	L.id [LearnerId],
	L.firstname [Learner Firstname], 
	L.lastname [Learner Lastname],
	L.middlename [Learner Middlename],
	C.id [CoachId],
	C.firstname [Coach Firstname],
	C.lastname [Coach Lastname],
	C.middlename [Coach Middlename],
	Sport.name [Sport]
from
	Learner
inner join Person as L on L.id = Learner.apprentice_id
inner join Person as C on C.id = Learner.coach_id
inner join Experience on Experience.person_id = L.id
inner join Sport on Sport.id = Experience.sport_id
go

create view AllParticipants
as
select
	Person.id [PersonId],
	Person.firstname [Firstname],
	Person.lastname [Lastname],
	Person.middlename [Middlename],
	Participant.id [ParticipantId],
	Participant.results [Scores],
	Club.id [ClubId],
	Club.name [Club],
	Competition.id [CompetitionId],
	Competition.competition_date [Competiton Date],
	Competition.name [Competition Name]
from 
	Person
inner join Experience on Experience.person_id = Person.id
inner join Participant on Participant.experience_id = Experience.id
inner join Competition on Participant.competition_id = Competition.id
inner join Club on Club.id = Participant.club_id
go

/* List of all buildling by type etc */
create procedure BuildingByType
	@buildingType as varchar(30)
as
	select Building.id [Id],
	       Organization.name [Owner Organization],
		   Building.name [Name],
		   Building.building_address [Address],
		   Building.building_type [Type],
		   Building.places [Places],
		   Building.area [Area]
	from Building 
	inner join Organization on Organization.id = Building.organization_id	   
	where building_type = @buildingType
go

/* List of all building which has more than 100 places */
create procedure BuildingWithPlaces
	@placesAmount as int
as
	select Building.id [Id],
	       Organization.name [Owner Organization],
		   Building.name [Name],
		   Building.building_address [Address],
		   Building.building_type [Type],
		   Building.places [Places],
		   Building.area [Area]
	from Building
	inner join Organization on Organization.id = Building.organization_id
	where places > @placesAmount
go

/* List sportsmen with particular sport */
create procedure SportsmenWithParticularSport
	@sportName as varchar(30)
as
	select 
		   [PersonId],
		   [Firstname],
		   [Lastname],
		   [Middlename],
		   [Sport]
	from SportsmenWithSports where SportsmenWithSports.Sport = @sportName
go

/* Sportsmen who are trained by concreete coach */
create procedure SportsmenOfCoach
	@firstname as varchar(30),
	@lastname as varchar(30)
as
	select 
		   [LearnerId] AS Id,
		   [Learner Firstname] AS Firstname,
		   [Learner Lastname] AS Lastname,
		   [Learner Middlename] AS Middlename
	from SportsmenWithCoaches 
	where 
		SportsmenWithCoaches.[Coach Firstname] = @firstname
		and SportsmenWithCoaches.[Coach Lastname ] = @lastname
go

/* Sportsmen who has qualification */
create procedure SportsmenWithQualification
	@sportTitle as varchar(30)
as
	select 
	       PersonId,
		   Firstname,
		   Lastname,
		   Middlename,
		   Birthdate,
		   Title
	from SportsmenWithSports where SportsmenWithSports.Title = @sportTitle
go

/* Sportsmen who study more than one sports */
create procedure ThoseWhoStudyMoreThanOneSport
as
	select 
		SportsmenWithSports.Firstname [Firstname],
		SportsmenWithSports.Lastname [Lastname],
		SportsmenWithSports.Middlename [Middlename],
		COUNT(SportsmenWithSports.Sport)
	from SportsmenWithSports
	group by SportsmenWithSports.Firstname, SportsmenWithSports.Lastname, SportsmenWithSports.Middlename
	having count(SportsmenWithSports.Sport) > 1
go

/* Coaches of sportsman */
create procedure CoachesOfSportsman
	@firstname as varchar(30),
	@lastname as varchar(30)
as
	select distinct
		SportsmenWithCoaches.[Coach Firstname] AS Firstname,
		SportsmenWithCoaches.[Coach Lastname ] AS Lastname,
		SportsmenWithCoaches.[Coach Middlename] AS Middlename
	from SportsmenWithCoaches
	where
		SportsmenWithCoaches.[Learner Firstname] = @firstname
		and SportsmenWithCoaches.[Learner Lastname] = @lastname
go

/* Competitions between date */
create procedure CompetitionsBetweenDate
	@from as date,
	@to as date
as
	select 
		AllCompetitions.CompetitionName,
		AllCompetitions.CompetitionDate
	from AllCompetitions
	where AllCompetitions.CompetitionDate between @from and @to
go

/* Competitions by organization */
create procedure CompetitionsByOrganization
	@organizationName as varchar(30)
as
	select 
		AllCompetitions.CompetitionName,
		AllCompetitions.CompetitionDate,
		AllCompetitions.OrganizationName
	from AllCompetitions
	where AllCompetitions.OrganizationName = @organizationName
go

/* Competitions in specific building */
create procedure CompetitionsInBuilding
	@buildingName as varchar(30)
as
	select 
		AllCompetitions.CompetitionName,
		AllCompetitions.BuildingName
	from AllCompetitions
	where AllCompetitions.BuildingName = @buildingName
go

/* Competitions by specific sport */
create procedure CompetitionsSport
	@sportName as varchar(30)
as
	select 
		AllCompetitions.CompetitionName,
		AllCompetitions.SportName
	from AllCompetitions
	where AllCompetitions.SportName = @sportName
go

/* Coaches by sport */
create procedure CoachesSport
	@sportName as varchar(30)
as
	select distinct
		SportsmenWithCoaches.[Coach Firstname] AS Firstname,
		SportsmenWithCoaches.[Coach Lastname] AS Lastname,
		SportsmenWithCoaches.[Coach Middlename] AS Middlename
	from SportsmenWithCoaches
	where SportsmenWithCoaches.Sport = @sportName
go

/* The amount of competitions by organizations between date */
create procedure CompetitonsByOrganizationBetweenDate
	@from date,
	@to date
as
	select
		AllCompetitions.OrganizationName,
		COUNT(OrganizationName) AS AmountOfCompetitions
	from AllCompetitions
	where AllCompetitions.CompetitionDate between @from and @to
	group by
		AllCompetitions.OrganizationName
go

/* Competitions on building between date */
create procedure CompetitionsOnBuildingBetweenDate
	@from date,
	@to date
as
	select
		AllCompetitions.BuildingName,
		AllCompetitions.CompetitionName,
		AllCompetitions.SportName,
		AllCompetitions.CompetitionDate
	from AllCompetitions
	where
		AllCompetitions.CompetitionDate between @from and @to
go

/* Winners of competition */
create procedure CompetitionWinners
	@competitionName varchar(30)
as
	select 
		AllParticipants.Firstname,
		AllParticipants.Lastname,
		AllParticipants.Middlename,
		AllParticipants.Scores
	from AllParticipants
	where AllParticipants.Scores > 75
	and	  AllParticipants.[Competition Name] = @competitionName
go

/* Get all clubs and amount of sportsmen who took part in competitions between date */
create procedure ClubSportsmensAmountInCompetitions
	@from date,
	@to date
as
	select 
		AllParticipants.Club,
		COUNT(*) AS AmountOfSportsmen
	from AllParticipants
	where AllParticipants.[Competiton Date] between @from and @to
	group by AllParticipants.Club
go

/* Sportsmen who didn't take part in competitions between date */
create procedure ThoseWhoDidntTakePartInCompetitions
	@from date,
	@to date
as
	select 
		SportsmenWithSports.Firstname,
		SportsmenWithSports.Lastname,
		SportsmenWithSports.Middlename
	from SportsmenWithSports
	where not exists
		(select * from AllParticipants
		where AllParticipants.[Competiton Date] between @from and @to
		and AllParticipants.Firstname = SportsmenWithSports.Firstname
		and AllParticipants.Lastname = SportsmenWithSports.Lastname
		and AllParticipants.Middlename = SportsmenWithSports.Middlename)
go
