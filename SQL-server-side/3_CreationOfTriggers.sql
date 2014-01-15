use master
go

use SportInfrastructure
go

create trigger OnAddParticipant
on Participant
instead of insert
as
begin
	declare @competitionId int
	declare @personId int
	select @competitionId = inserted.competition_id, @personId = inserted.person_id from inserted

	declare @competitionDate date
	select @competitionDate = Competition.competition_date from Competition	where Competition.id = @competitionId

	if (exists(select [Person Id], [Competiton Date] from AllParticipants where [Competiton Date] = @competitionDate and [Person Id] = @personId))
	begin 
		rollback transaction
		return
	end

	insert into Participant (person_id, competition_id, club_id, results) select person_id, competition_id, club_id, results from inserted
end
go

create trigger AddCompetition
on Competition
for insert
as
begin
declare @buildingId int
declare @competitionDate date
declare @competitionId int
select @buildingId = inserted.building_id, @competitionDate = inserted.competition_date, @competitionId = inserted.id from inserted 
if exists(select * from Competition where Competition.building_id = @buildingId and Competition.competition_date = @competitionDate and Competition.id <> @competitionId)
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
	where [Building Name] = @buildingName
	and DATEDIFF(DAY, AllCompetitions.[Competition Date], GETDATE()) <= 7)
begin rollback transaction end
go
