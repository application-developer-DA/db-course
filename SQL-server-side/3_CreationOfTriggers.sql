use master
go

use SportInfrastructure
go

create trigger OnAddParticipant
on Participant
after insert
as
declare @competitionId int
declare @experienceId int
select 
	@competitionId = inserted.competition_id,
	@experienceId = inserted.experience_id
from inserted
declare @competitionDate date
	select @competitionDate = Competition.competition_date
	from Competition
	where Competition.id = @competitionId
declare @firstName varchar
declare @lastName varchar
declare @middleName varchar
select 
	@firstName = SportsmenWithSports.Firstname,
	@lastName = SportsmenWithSports.Lastname,
	@middleName = SportsmenWithSports.Middlename
from
	SportsmenWithSports
where SportsmenWithSports.ExperienceId = @experienceId
if exists(
	select
		AllParticipants.Firstname,
		AllParticipants.Lastname,
		AllParticipants.Middlename,
		AllParticipants.[Competiton Date]
	from
		AllParticipants
	where AllParticipants.[Competiton Date] = @competitionDate
	and AllParticipants.Firstname = @firstName
	and AllParticipants.Lastname = @lastName
	and AllParticipants.Middlename = @middleName)
begin rollback transaction end
go

create trigger AddCompetition
on Competition
for insert
as
begin
declare @buildingId int
declare @competitionDate date
select @buildingId = inserted.building_id, @competitionDate = inserted.competition_date from inserted 
if exists(select * from Competition where Competition.building_id = @buildingId and Competition.competition_date = @competitionDate)
	begin rollback transaction end
end
go

create trigger OnTitleChanged
on Experience
for update
as 
declare @title varchar(30)
declare @experienceId int
select @experienceId = inserted.id, @title = inserted.title from inserted
declare @currentTitle varchar(30)
select @currentTitle = Experience.title from Experience where Experience.id = @experienceId
if @currentTitle = 'Beginner'
begin
	if @title <> 'Intermediate'
		begin rollback transaction end
end
else if @currentTitle = 'Intermediate'
begin
	if @title <> 'Proficient'
		begin rollback transaction end
end
else if @currentTitle = 'Proficient'
begin
	if @title <> 'Coach'
		begin rollback transaction end
end
else if @currentTitle = 'Coach'
begin
	begin rollback transaction end
end
go

create trigger OnBuildlingDelete
on Building
for delete
as
declare @buildingName varchar
select @buildingName = inserted.name from inserted
if exists(select * from AllCompetitions 
	where AllCompetitions.BuildingName = @buildingName
	and DATEDIFF(DAY, AllCompetitions.CompetitionDate, GETDATE()) <= 7)
begin rollback transaction end
go