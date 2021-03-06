--[[ ------------------------------------------------
	JKGalaxies Lua Code
	Admin system
		
	written by eezstreet
--------------------------------------------------]]

--[[ ------------------------------------------------
	Accounts
--------------------------------------------------]]

admins = { }
ranks = { }
-- Never access sortedadmins/sortedranks directly, it's used for saving et al
sortedadmins = { }
sortedranks = { }

-- Felt like refactoring this, SO THEN I DOUBLED IT
permissions = { }
sortedpermissions = { }
local numPermissions = 0

--[[ ------------------------------------------------
	AdmPerm_AddPermission( permissionname, permissiondefault, friendlyname, color )
	Adds a permission to the permission table.
--------------------------------------------------]]

local function AdmPerm_AddPermission( permissionname, permissiondefault, friendlyname, color )
	local permission = { }
	permission["name"] = permissionname
	permission["default"] = permissiondefault
	permission["friendlyname"] = friendlyname
	permission["color"] = color

	permissions[permissionname] = permission
	table.insert( sortedpermissions, numPermissions, permission )

	numPermissions = numPermissions + 1
end

--[[ ------------------------------------------------
	AdmPerm_InitPermissions()
	Creates all of the standard permissions.
--------------------------------------------------]]

-- High-Risk (Red/^1): Kicking/Banning, mostly. These are dangerous and should only be given to high-ranking admins.
-- Harmless (Green/^2): Status, listing stuff, help, etc. These aren't harmful from a security standpoint.
-- Self-Exploitable (Yellow/^3): From a security standpoint, these are only harmful to the person who uses it. Changing details falls into this category.
-- Building Commands (Blue/^4): Building stuff. Built stuff can be destroyed by the users (later on, when we implement that, anyway), so no harm done.
-- Communication (Cyan/^5): Talking with clients, and amongst admins. Can be extremely annoying if spoofed.
-- Structural (Magenta/^6): Alters the structure of administration, such as dealing with ranks. Not necessarily needed on accounts, but they're there for convenience.
-- Annoying (Orange/^8): While not necessarily harmful, if used, they can be annoying. Stuff like slap, slay, etc falls into this category.

local function AdmPerm_InitPermissions( )
	AdmPerm_AddPermission( "can-changedetails", 		1, "admchangedetails", 		"^3" )
	AdmPerm_AddPermission( "can-addaccounts", 		0, "admnewaccount", 		"^6" )
	AdmPerm_AddPermission( "can-deleteaccounts", 		0, "admdeleteaccount", 		"^6" )
	AdmPerm_AddPermission( "can-list-online", 		1, "admlist online", 		"^2" )
	AdmPerm_AddPermission( "can-list-admins", 		1, "admlist admins", 		"^2" )
	AdmPerm_AddPermission( "can-list-powers", 		1, "admlist ranks", 		"^2" )
	AdmPerm_AddPermission( "can-list-ranks", 		1, "admlist powers", 		"^2" )
	AdmPerm_AddPermission( "can-list-permissions", 		0, "admlist permissions", 	"^2" )
	AdmPerm_AddPermission( "can-rank-inspect", 		1, "admrank inspect", 		"^2" )
	AdmPerm_AddPermission( "can-rank-create", 		0, "admrank create", 		"^1" )
	AdmPerm_AddPermission( "can-rank-delete", 		0, "admrank delete", 		"^1" )
	AdmPerm_AddPermission( "can-rank-addpermission", 	0, "admrank addpermission", 	"^1" )
	AdmPerm_AddPermission( "can-rank-deletepermission", 	0, "admrank deletepermission", 	"^1" )
	AdmPerm_AddPermission( "can-alter-rank", 		0, "admalter rank", 		"^6" )
	AdmPerm_AddPermission( "can-alter-password", 		0, "admalter password", 	"^6" )
	AdmPerm_AddPermission( "can-status", 			1, "admstatus", 		"^2" )
	AdmPerm_AddPermission( "can-say", 			1, "admsay", 			"^5" )
	AdmPerm_AddPermission( "can-tell", 			1, "admtell", 			"^5" )
	AdmPerm_AddPermission( "can-speak", 			1, "admspeak", 			"^5" )
	AdmPerm_AddPermission( "can-puppet", 			0, "admpuppet", 		"^8" )
	AdmPerm_AddPermission( "can-place", 			0, "bPlace", 			"^4" )
	AdmPerm_AddPermission( "can-delent", 			0, "bDelent", 			"^4" )
	AdmPerm_AddPermission( "can-entcount", 			1, "bEntCount", 		"^4" )
	AdmPerm_AddPermission( "can-showspawnvars", 		0, "bShowSpawnVars", 		"^4" )
	AdmPerm_AddPermission( "can-rotate", 			0, "bRotate", 			"^4" )
	AdmPerm_AddPermission( "use-cheats", 			0, "Use Cheats", 		"^8" )
end

--[[ ------------------------------------------------
	AdmRank_GetRank
	Gets the rank of a player.
--------------------------------------------------]]

function AdmRank_GetRank(ply)
	if ply.IsAdmin then
		local account = ply:GetAdminAccount()
		return ranks[admins[account]["rank"]]
	end
	return nil
end

--[[ ------------------------------------------------
	AdmRank_InitRanks
	Inits all of the information about ranks
--------------------------------------------------]]

local function AdmRank_InitRanks( )
	-- Grab our file
	local jRoot = json.RegisterFile("server/ranks.json")
	
	if jRoot == nil then
		json.Clear()
		securitylog("Could not register ranklist.json")
		return
	end

	-- Load up the list of ranks
	local rankArray = json.GetObjectItem( jRoot, "ranks" )
	if rankArray == nil then
		-- Lol, does not exist.
		json.Clear()
		securitylog("ranks array in ranks.json nonexisting.")
		return
	end

	-- Loop through objects
	local i, j
	for i = 0, json.GetArraySize(rankArray)-1 do
		local jObject = json.GetArrayItem( rankArray, i )
		local rank = {}
		local jObjectItem

		jObjectItem = json.GetObjectItem( jObject, "name" )
		rank["name"] = json.ToString( jObjectItem )

		--
		-- INSERT COMMAND-BASED STUFF HERE
		--

		for j = 0, numPermissions-1 do
			local permname = sortedpermissions[j]["name"]
			local defvalue = sortedpermissions[j]["default"]

			jObjectItem = json.GetObjectItem( jObject, permname )
			rank[permname] = json.ToBooleanOpt( jObjectItem, defvalue )
		end
	
		--
		-- END COMMAND-BASED STUFF
		--

		rank["sorted"] = i

		ranks[rank["name"]] = rank
		table.insert(sortedranks, i, rank)
	end

	ranks["numRanks"] = json.GetArraySize(rankArray)

	-- Make sure to clean up our mess.
	json.Clear()
end

--[[ ------------------------------------------------
	AdmList_InitAdminList
	Opens up the json/savefile for the admins,
	and then applies this information to the ingame system
--------------------------------------------------]]

local function AdmList_InitAdminList ( )
	-- Grab our file
	local jRoot = json.RegisterFile("server/adminlist.json")

	if jRoot == nil then
		json.Clear()
		securitylog("Could not register adminlist.json")
		return
	end

	-- Instead of using a global Lua variable, let's load up a list of users
	local adminArray = json.GetObjectItem( jRoot, "admins" )
	if adminArray == nil then
		-- Lol, does not exist.
		json.Clear()
		securitylog("admins array in adminlist.json nonexisting.")
		return
	end

	-- ok, so loop through the objects
	local i
	for i = 0, json.GetArraySize(adminArray)-1 do
		local jObject = json.GetArrayItem( adminArray, i )
		local admin = {}
		local jObjectItem

		jObjectItem = json.GetObjectItem( jObject, "username" )
		admin["username"] = json.ToString( jObjectItem )
		
		jObjectItem = json.GetObjectItem( jObject, "password" )
		admin["password"] = json.ToString( jObjectItem )

		jObjectItem = json.GetObjectItem( jObject, "rank" )
		admin["rank"] = json.ToString( jObjectItem )

		if ranks[admin["rank"]] == nil then
			securitylog("WARNING: admin account " .. admin["username"] .. " pointing to invalid rank " .. admin["rank"])
			print("^3WARNING: admin account " .. admin["username"] .. " pointing to invalid rank " .. admin["rank"])
		end

		admin["sorted"] = i

		admins[admin["username"]] = admin
		table.insert(sortedadmins, i, admin)
		
	end

	admins["numAdmins"] = json.GetArraySize(adminArray)

	-- Make sure to clean up our mess.
	json.Clear()
end

--[[ ------------------------------------------------
	AdminHelp_ListPowers
	Lists powers for a given rank, or, lists
	all powers. OR, this can be used on
	permissions also. It's like a 4-in-1 func.
--------------------------------------------------]]

local function AdminHelp_ListPowers( rank, listpermissions, listall )
	-- This code is used in multiple places, I figured it would be wise to put this into a function
	if rank == nil then
		return
	end

	local printedtext = ""
	local i

	-- Loop through everything.
	for i = 0, numPermissions-1 do
		local perm = sortedpermissions[i]
		local printcolor = perm["color"]
		local permname = perm["name"]
		local addtext

		-- Make sure we have this permission
		if listall == true or rank[permname] then
			if listpermissions == true then
				addtext = permname
			else
				addtext = perm["friendlyname"]
			end
			printedtext = printedtext .. printcolor .. addtext .. "^7, "
		end
	end
						
	return printedtext
end

--[[ ------------------------------------------------
	RankPermission_IsValid
	Checks if a permission is valid or not
--------------------------------------------------]]

local function RankPermission_IsValid( permissionname )
	if permissions[permissionname] ~= nil then
		return true
	else
		return false
	end
end

--[[ ------------------------------------------------
	RankList_SaveRanks
	Saves all the ranks into a file.
--------------------------------------------------]]

local function RankList_SaveRanks( reason )
	-- Save the .json file
	json.RegisterStream( 3, 1 )

	-- Root object
	json.BeginObject( "0" )

	-- "ranks" array
	json.BeginArray( "ranks" )

	-- Okay, now to save all of the ranks...
	local k, l
	for k = 0, ranks["numRanks"]-1 do
		-- Array base object
		json.BeginObject( "0" )

		local sortedrank = sortedranks[k]

		json.WriteString( "name", sortedrank["name"] )

		for l = 0, numPermissions-1 do
			local permission = sortedpermissions[l]
			local permname = permission["name"]

			if sortedrank[permname] ~= nil then
				json.WriteBoolean( permname, sortedrank[permname] )
			else
				json.WriteBoolean( permname, permission["default"] )
			end
		end
		
		-- Array base object
		json.EndObject( )
	end

	-- "ranks" array
	json.EndObject()

	-- Root object
	json.EndObject()

	-- Now save it all up
	json.FinishStream( "server/ranks.json" )

	-- Log it to the security log
	securitylog("Ranks saved. (Reason: " .. reason .. " )")
end

--[[ ------------------------------------------------
	UpdateRank
	Updates the savefile.
--------------------------------------------------]]

local function UpdateRank( rank, reason )
	ranks[rank["name"]] = rank
	if sortedranks[rank["sorted"]] ~= nil then
		sortedranks[rank["sorted"]] = account
	else
		-- Likely a new rank
		table.insert(sortedranks, ranks["numRanks"]-1, rank)
	end
	RankList_SaveRanks( reason )
end

--[[ ------------------------------------------------
	RankList_AddRank
	Adds a new rank into the system and
	saves the ranks.
--------------------------------------------------]]

local function RankList_AddRank( rank )
	ranks["numRanks"] = ranks["numRanks"] + 1
	table.insert(sortedranks, ranks["numRanks"]-1, rank)
	UpdateRank( rank, "Added new rank: " .. rank["name"] )
end

--[[ ------------------------------------------------
	RankList_DeleteRank
	Deletes a rank and saves the ranks.
--------------------------------------------------]]

local function RankList_DeleteRank( rank )
	local rankname = rank["name"]
	table.remove(sortedranks, rank["sorted"])
	ranks[rank["name"]] = nil
	-- One sec, make sure we aren't deleting the last rank...
	if ranks["numRanks"] == 1 then
		print("^1ERROR: admin tried to delete last rank...")
		return
	end
	-- Okay, we need to change the sorted stuff now
	local k
	for k = 0, ranks["numRanks"]-1 do
		if sortedranks[k] ~= nil and ranks[sortedranks[k]] ~= nil then
			ranks[sortedranks[k]]["sorted"] = k
		end
	end

	ranks["numRanks"] = ranks["numRanks"] - 1
	RankList_SaveRanks( "Deleted rank: " .. rankname )
end

--[[ ------------------------------------------------
	AdmList_SaveAccounts
	Saves all the admin accounts into a file.
--------------------------------------------------]]

local function AdmList_SaveAccounts( reason )
	-- Save the .json file
	json.RegisterStream( 3, 1 )
	
	-- Root object
	json.BeginObject( "0" )

	-- "admins" array
	json.BeginArray( "admins" )

	-- Okay, now we're going to save all the accounts...
	local k
	for k = 0, admins["numAdmins"]-1 do

		-- Array base object
		json.BeginObject( "0" )

		local sortedaccount = sortedadmins[k]

		json.WriteString( "username", sortedaccount["username"] )
		json.WriteString( "password", sortedaccount["password"] )
		json.WriteString( "rank", sortedaccount["rank"] )

		-- Array base object
		json.EndObject( )
	end

	-- "admins" array
	json.EndObject( )

	-- Root object
	json.EndObject( )

	-- Now save it all up
	json.FinishStream( "server/adminlist.json" )

	-- Log it to the security log
	securitylog("Accounts saved. (Reason: " .. reason .. " )")
end

--[[ ------------------------------------------------
	UpdateAccount
	Updates the accounts and calls AdmList_SaveAccounts
--------------------------------------------------]]

local function UpdateAccount( account, reason )
	admins[account["username"]] = account
	if sortedadmins[account["sorted"]] ~= nil then
		sortedadmins[account["sorted"]] = account
	else
		-- Likely a new account
		table.insert(sortedadmins, admins["numAdmins"]-1, account)
	end
	AdmList_SaveAccounts( reason )
end

--[[ ------------------------------------------------
	AdmList_AddAccount
	Adds a new account into the system
	Also saves all the accounts.
--------------------------------------------------]]

local function AdmList_AddAccount( account )
	admins["numAdmins"] = admins["numAdmins"] + 1
	table.insert(sortedadmins, admins["numAdmins"]-1, account)
	UpdateAccount( account, "Added new account: " .. account["username"] .. " [Rank: " .. account["rank"] .. "]" )
end

--[[ ------------------------------------------------
	AdmList_DeleteAccount
	Deletes an account and saves all accounts.
--------------------------------------------------]]

local function AdmList_DeleteAccount( account )
	local username = account["username"]
	table.remove(sortedadmins, account["sorted"])
	admins[account["username"]] = nil
	-- Okay, we need to change the sorted stuff now
	local k
	for k = 0, admins["numAdmins"]-1 do
		if sortedadmins[k] ~= nil and admins[sortedadmins[k]] ~= nil then
			admins[sortedadmins[k]]["sorted"] = k
		end
	end

	admins["numAdmins"] = admins["numAdmins"] - 1
	AdmList_SaveAccounts( "Deleted account: " .. username )
end

--[[ ------------------------------------------------
	InitAdminAccounts
	Inits ranks, admin list, permissions, etc
--------------------------------------------------]]

local function InitAdminAccounts( )
	AdmPerm_InitPermissions()
	AdmRank_InitRanks()
	AdmList_InitAdminList()
end

--[[ ------------------------------------------------
	Commands
--------------------------------------------------]]

local function AdmReply(ply, message)
	ply:SendChat("^7Admin System: " .. message)
end

local function Admin_Login(ply, argc, argv)
	if argc ~= 3 then
		-- Invalid number of arguments
		AdmReply(ply, "^1Invalid arguments")
		return
	end
	if ply.IsAdmin then
		-- We are admin, so no need to re-log us
		AdmReply(ply, "^5You are already logged in")
		return
	end
	if admins[argv[1]] ~= nil then
		if admins[argv[1]]["password"] == argv[2] then
			ply.IsAdmin = true
			ply:SetAdminAccount(argv[1])
			AdmReply(ply, "^2Admin login successful")

			local account = admins[argv[1]]
			local rank = ranks[account["rank"]]

			if rank["use-cheats"] == true then
				-- We can use cheats. Alright, mang! Let's set things up so that the C++ code knows that we can...
				ply.CanUseCheats = true
			end
			return
		end
	end
	AdmReply(ply, "^1Bad username/password")
end

local function Admin_Logout(ply, argc, argv)
	local k,v
	if ply.IsAdmin then
		ply.IsAdmin = false
		ply:SetAdminAccount(" ")
		if ply.CanUseCheats then
			ply.CanUseCheats = false -- don't want us using cheats when we shouldn't be able to...
		end
		AdmReply(ply, "^5You are now logged out")
		return
	end
	chatcmds.Ignore()
end

local function Admin_Kick(ply, argc, argv)
	local k,v
	if ply.IsAdmin then
		if argc < 2 then
			AdmReply(ply, "^1Please specify a player to kick")
			return
		end
		local target =  players.GetByArg(argv[1])
		if not target then
			AdmReply(ply, "^1Invalid player specified")
			return
		end
		local reason
		if argc > 2 then
			reason = table.concat(argv," ",2, argc-1)
		else 
			reason = nil
		end
		AdmReply(ply, "^5Player " .. target:GetName() .. " ^5has been kicked")
		target:Kick(reason)
		return
	end
	chatcmds.Ignore()
end

local function Admin_ChangeDetails(ply, argc, argv)
	if ply.IsAdmin then
		local rank = AdmRank_GetRank(ply)
		if rank["can-changedetails"] == true then
			-- Change our account details then?
			if argc < 2 then
				AdmReply(ply, "Please specify which details you'd like to change: password")
			elseif argv[1] == "password" or argv[1] == "Password" then
				-- We want to change the password on this account.
				-- Syntax:
				-- /admchangedetails password <oldpass> <newpass>
				if argc ~= 4 then
					AdmReply(ply, "Syntax: /admchangedetails password <oldpass> <newpass>")
				else
					local accountname = ply:GetAdminAccount()
					local account = admins[accountname]
					if argv[2] ~= account["password"] then
						AdmReply(ply, "Your old password did not match correctly. Please try again.")
					else
						-- kk, change.
						account["password"] = argv[3]
						UpdateAccount( account, account["username"] .. " changed their password" )
						AdmReply(ply, "^4Password changed.")
					end
				end
			else
				AdmReply(ply, "Invalid details type specified. Valid details: password")
			end
		else
			AdmReply(ply, "^4You don't have permission to perform this action.")
		end
	else
		AdmReply(ply, "^1You are not logged in.")
	end
end

local function Admin_Profile(ply, argc, argv)
	if ply.IsAdmin then
		local account = ply:GetAdminAccount()
		AdmReply(ply, "^4You are logged in as " .. account .. " [Rank: " .. admins[account]["rank"] .. "]")
		return
	end
	chatcmds.Ignore()
end

local function Admin_AddAccount(ply, argc, argv)
	if ply.IsAdmin then
		-- Ya they're an admin...but are they authorized to do so?
		local rank = AdmRank_GetRank(ply)
		if rank["can-addaccounts"] ~= true then
			AdmReply(ply, "^1You do not have permission to perform this action.")
		else
			-- Syntax:
			-- /admnewaccount <username> <password> <rank>
			if argc ~= 4 then
				AdmReply(ply, "^3Syntax: /admnewaccount <username> <password> <rank>")
			else
				if admins[argv[1]] ~= nil then
					AdmReply(ply, "^1Could not create account, an account by this name already exists.")
				else
					local newaccount = {}
					newaccount["username"] = argv[1]
					newaccount["password"] = argv[2]
					newaccount["rank"] = argv[3]
					if ranks[newaccount["rank"]] ~= nil then
						AdmList_AddAccount( newaccount )
						AdmReply(ply, "^4Account added.")
					else
						AdmReply(ply, "^3The account you added does not have a valid rank. Please add the rank and try again." )
					end
				end
			end
		end
	else
		AdmReply(ply, "^1You are not logged in.")
	end
end

local function AdmHelp_LogoutAccount(account)
	local k = 0
	while players.GetByID(k) ~= nil do
		ply = players.GetByID(k)
		if ply.IsAdmin then
			if account["username"] == ply:GetAdminAccount() then
				-- soz you've been booted
				AdmReply(ply, "^6Your account has been deleted. You have been logged out.")
				ply.IsAdmin = false
				ply:SetAdminAccount(" ")
			end
		end

		k = k + 1
	end
end

local function Admin_DeleteAccount(ply, argc, argv)
	if ply.IsAdmin then
		local rank = AdmRank_GetRank(ply)
		if rank["can-deleteaccounts"] ~= true then
			AdmReply(ply, "^1You do not have permission to perform this action.")
		else
			-- Syntax:
			-- /admdeleteaccount <username>
			if argc ~= 2 then
				AdmReply(ply, "^3Syntax: /admdeleteaccount <username>")
			else
				local desiredaccount = admins[argv[1]]
				if desiredaccount ~= nil then
					local ouraccount = admins[ply:GetAdminAccount()]
					if ouraccount["username"] ~= argv[1] then
						AdmHelp_LogoutAccount(desiredaccount)
						AdmList_DeleteAccount(desiredaccount)
						AdmReply(ply, "^4Account deleted.")
					else
						AdmReply(ply, "^1You cannot delete your own account.")
					end
				else
					AdmReply(ply, "^1Cannot delete account, it does not exist.")
				end
			end
		end
	else
		AdmReply(ply, "^1You are not logged in.")
	end
end

local function Admin_List(ply, argc, argv)
	if ply.IsAdmin then
		if argc < 2 then
			AdmReply(ply, "^3Syntax: /admlist <online/admins/ranks/powers>")
		else
			local rank = AdmRank_GetRank(ply)
			if argv[1] == "online" then
				if rank["can-list-online"] ~= true then
					AdmReply(ply, "^1You do not have permission to perform this action.")
				else
					local k = 0
					local printstring = ""
					AdmReply(ply, "^4Results printed to console.")
					while players.GetByID(k) ~= nil do
						local plysel = players.GetByID(k)
						if plysel:IsValid() then
							if plysel.IsAdmin then
								local plyselname = plysel:GetName()
								local plyselaccountname = plysel:GetAdminAccount()
								local plyselaccount = admins[plyselaccountname]
								if plyselaccount ~= nil then
									printstring = printstring .. plysel:GetName() .. " [" .. plyselaccountname .. "--" .. plyselaccount["rank"] .. "], "
								end
							end
						end
						k = k + 1
					end
					ply:SendPrint(printstring)
				end
			elseif argv[1] == "admins" then
				if rank["can-list-admins"] ~= true then
					AdmReply(ply, "^1You do not have permission to perform this action.")
				else
					AdmReply(ply, "^4Results printed to console.")
					ply:SendPrint("^2All admin accounts:")
					local k
					local printstring = ""
					for k = 0, admins["numAdmins"]-1 do
						printstring = printstring .. sortedadmins[k]["username"] .. " [" .. sortedadmins[k]["rank"] .. "], "
					end
					ply:SendPrint(printstring)
				end
			elseif argv[1] == "ranks" then
				if rank["can-list-ranks"] ~= true then
					AdmReply(ply, "^1You do not have permission to perform this action.")
				else
					AdmReply(ply, "^4Results printed to console.")
					ply:SendPrint("^2All ranks:")
					local k
					local printstring = ""
					for k = 0, ranks["numRanks"]-1 do
						printstring = printstring .. sortedranks[k]["name"] .. ", "
					end
					ply:SendPrint(printstring)
				end
			elseif argv[1] == "powers" then
				if rank["can-list-powers"] ~= true then
					AdmReply(ply, "^1You do not have permission to perform this action.")
				else
					AdmReply(ply, "^4Results printed to console.")
					ply:SendPrint("^2Your powers:")
					
					ply:SendPrint(AdminHelp_ListPowers( rank, false, false ))
				end
			elseif argv[1] == "permissions" then
				if rank["can-list-permissions"] ~= true then	
					AdmReply(ply, "^1You do not have permission to perform this action.")
				else
					AdmReply(ply, "^4Results printed to console.")
					ply:SendPrint("^2All permissions:")

					ply:SendPrint(AdminHelp_ListPowers( rank, true, true ))
				end
			else
				AdmReply(ply, "^3Unknown admlist mode. Valid modes are online, admins, ranks, powers")
			end
		end
	else
		AdmReply(ply, "^1You are not logged in.")
	end
end

local function Admin_Rank(ply, argc, argv)
	if ply.IsAdmin then
		if argc < 2 then
			AdmReply(ply, "^3Syntax: /admrank <inspect> [...args...]")
		else
			local rank = AdmRank_GetRank(ply)
			if argv[1] == "inspect" then
				if argc < 3 then
					AdmReply(ply, "^3Syntax: /admrank inspect <rank>")
				elseif rank["can-rank-inspect"] ~= true then
					AdmReply(ply, "^1You do not have permission to perform this action.")
				else
					local inspectedrank = ranks[argv[2]]
					if inspectedrank == nil then
						AdmReply(ply, "^1'" .. argv[2] .. "' is not a valid rank.")
					else
						AdmReply(ply, "^4Rank permissions printed to console.")
						ply:SendPrint("^5This rank (" .. argv[2] .. ") can...")
						
						ply:SendPrint(AdminHelp_ListPowers( inspectedrank, false, false ))

						ply:SendPrint("^5Rank permissions:")

						ply:SendPrint(AdminHelp_ListPowers( inspectedrank, true, false ))
					end
				end
			elseif argv[1] == "create" then
				if argc < 3 then
					AdmReply(ply, "^3Syntax: /admrank create <rank name>")
				elseif rank["can-rank-create"] ~= true then
					AdmReply(ply, "^1You do not have permission to perform this action.")
				else
					local blankrank = {}
					blankrank["name"] = argv[2]
					RankList_AddRank( blankrank )
					AdmReply(ply, "^4Rank successfully added.")
				end
			elseif argv[1] == "delete" then
				if argc < 3 then
					AdmReply(ply, "^3Syntax: /admrank delete <rank> [change admins using this rank to THIS rank]")
				elseif rank["can-rank-delete"] ~= true then
					AdmReply(ply, "^1You do not have permission to perform this action.")
				else
					local rank = ranks[argv[2]]
					if rank == nil then
						AdmReply("^1ERROR: Invalid rank specified.")
					end

					-- Loop through the admins and make sure none of us are using this rank...
					local k
					for k = 0, admins["numAdmins"]-1 do
						if sortedadmins[k]["rank"]["name"] == argv[2] then
							if argc < 4 then
								AdmReply(ply, "^3This rank is in use. You will need to specify a rank for people to use, since this rank is gone.")
								return
							end
							sortedadmins[k]["rank"] = ranks[argv[3]]
						end
					end

					-- K, guess it's okay to kill the rank, now...
					-- You will be missed, rank.
					RankList_DeleteRank( rank )
					AdmReply(ply, "^4Rank deleted.")
				end
			elseif argv[1] == "addpermission" then
				if argc < 4 then
					AdmReply(ply, "^3Syntax: /admrank addpermission <rank> <permission>")
				elseif rank["can-rank-addpermission"] ~= true then
					AdmReply(ply, "^1You do not have permission to perform this action.")
				else
					local rank = ranks[argv[2]]
					if rank == nil then
						AdmReply(ply, "^1Invalid rank specified.")
						return
					end

					local permissionname = argv[3]
					if RankPermission_IsValid( permissionname ) == false then
						AdmReply(ply, "^1Invalid permission specified.")
						return
					end

					rank[permissionname] = true
					UpdateRank( rank, ply:GetAdminAccount() .. " added permission '" .. permissionname .. "' to rank " .. rank["name"])
					AdmReply(ply, "^4Rank updated successfully.")
				end
			elseif argv[1] == "deletepermission" then
				if argc < 4 then
					AdmReply(ply, "^3Syntax: /admrank deletepermission <rank> <permission>")
				elseif rank["can-rank-deletepermission"] ~= true then
					AdmReply(ply, "^1You do not have permission to perform this action.")
				else
					local rank = ranks[argv[2]]
					if rank == nil then
						AdmReply(ply, "^1Invalid rank specified.")
						return
					end

					local permissionname = argv[3]
					if RankPermission_IsValid( permissionname ) == false then
						AdmReply(ply, "^1Invalid permission specified.")
						return
					end

					rank[permissionname] = false
					UpdateRank( rank, ply:GetAdminAccount() .. " deleted permission '" .. permissionname .. "' from rank " .. rank["name"])
					AdmReply(ply, "^4Rank updated successfully.")
				end
			else
				AdmReply(ply, "^3Unknown admrank mode, valid modes are: inspect, create, delete, addpermission, deletepermission")
			end
		end
	else
		AdmReply(ply, "^1You are not logged in.")
	end
end

local function Admin_Alter(ply, argc, argv)
	if ply.IsAdmin then
		local accountname = ply:GetAdminAccount()
		local rank = AdmRank_GetRank(ply)
		if argc ~= 4 then
			AdmReply(ply, "^3Syntax: /admalter <username> <rank/password> <change to..>")
		else
			local alteraccount = admins[argv[1]]
			if alteraccount == nil then
				AdmReply(ply, "^1Invalid username for /admalter.")
			else
				if argv[2] == "rank" then
					if rank["can-alter-rank"] ~= true then
						AdmReply(ply, "^1You do not have permission to perform this action.")
					else
						local newrank = argv[3]
						if ranks[newrank] == nil then
							AdmReply(ply, "^1Invalid rank.")
						else
							AdmReply(ply, "^4Account altered.")
							alteraccount["rank"] = argv[3]
							UpdateAccount( alteraccount, accountname .. " altered " .. alteraccount["username"] .. "'s rank (changed to " .. argv[3] .. ")" )
						end
					end
				elseif argv[2] == "password" then
					if rank["can-alter-password"] ~= true then
						AdmReply(ply, "^1You do not have permission to perform this action.")
					else
						AdmReply(ply, "^4Account altered.")
						alteraccount["password"] = argv[3]
						UpdateAccount( alteraccount, accountname .. " altered " .. alteraccount["username"] .. "'s password" )
					end
				else
					AdmReply(ply, "^3Invalid alter type. Valid types are rank, password.")
				end
			end
		end
	else
		AdmReply(ply, "^1You are not logged in.")
	end
end

local function Admin_Status(ply, argc, argv)
	if ply.IsAdmin then
		local rank = AdmRank_GetRank(ply)
		if rank["can-status"] ~= true then
			AdmReply(ply, "^1You do not have permission to perform this action.")
		else
			local k = 0
			AdmReply(ply, "^4Results printed to console.")
			ply:SendPrint("^2Status:")
			local printstring = ""
			while players.GetByID(k) ~= nil do
				local plysel = players.GetByID(k)
				if plysel:IsValid() then
					local plyselname = plysel:GetName()
					printstring = printstring .. k .. " - " .. plyselname
					if plysel.IsAdmin then
						local plyselaccountname = plysel:GetAdminAccount()
						local plyselaccount = admins[plyselaccountname]
						
						printstring = printstring .. " ^7(^2Logged in as ^4" .. plyselaccountname .. " ^5[Rank: " .. plyselaccount["rank"] .. "]^7)"
					end
				end
				k = k + 1
			end
			ply:SendPrint(printstring)
		end
	else
		AdmReply(ply, "^1You are not logged in.")
	end
end

local function Admin_Say(ply, argc, argv)
	if ply.IsAdmin then
		local rank = AdmRank_GetRank(ply)
		if rank["can-say"] ~= true then
			AdmReply(ply, "^1You do not have permission to perform this action.")
		else
			local accountname = ply:GetAdminAccount()
			local message = table.concat(argv," ",1, argc-1)
			chatmsg( "^7[^5" .. accountname .. "^7] <" .. rank["name"] .. "> " .. message )
		end
	else
		AdmReply(ply, "^1You are not logged in.")
	end
end

local function Admin_Tell(ply, argc, argv)
	if ply.IsAdmin then
		local rank = AdmRank_GetRank(ply)
		if rank["can-tell"] ~= true then
			AdmReply(ply, "^1You do not have permission to perform this action.")
		else
			if argc < 3 then
				AdmReply(ply, "^3Syntax: /admtell <player id/name> <message>")
			else
				local plytarg = players.GetByArg(argv[1])
				if plytarg == nil then
					AdmReply(ply, "^1Invalid player specified.")
					return
				end

				local account = admins[ply:GetAdminAccount()]
				local message = table.concat(argv," ",1, argc-1)

				plytarg:SendChat( "^5Admin " .. account["username"] .. " whispers: " .. message )

				if plytarg.lastadmtell == nil then
					plytarg:SendChat( "^8Reply to this message using /admreply <msg>" )
				end
				plytarg.lastadmtell = account["username"]
			end
		end
	else
		AdmReply(ply, "^1You are not logged in.")
	end
end

-- NOT to be confused with AdmReply!!
local function Admin_Reply(ply, argc, argv)
	if argc < 2 then
		AdmReply(ply, "^3Syntax: /admreply <message>")
		return
	end

	if ply.lastadmtell == nil then
		ply:SendChat( "^1You have not been messaged by an admin in this play session." )
		return
	end

	local account = ply.lastadmtell
	local k = 0
	local message = table.concat(argv," ",1, argc-1)
	while players.GetByID(k) ~= nil do
		local plytarg = players.GetByID(k)

		if( account == plytarg:GetAdminAccount() ) then
			plytarg:SendChat( "^5" .. ply.Name .. " replies: " .. message )
		end

		k = k + 1
	end

	ply:SendChat( "^5You reply: " .. message )
end

local function Admin_Speak(ply, argc, argv)
	if ply.IsAdmin then
		local rank = AdmRank_GetRank(ply)
		if rank["can-speak"] ~= true then
			AdmReply(ply, "^1You do not have permission to perform this action.")
			return
		end

		if argc < 2 then
			AdmReply(ply, "^3Syntax: /admspeak <message>")
			return
		end

		local k = 0
		local message = table.concat(argv," ",1, argc-1)
		local ouraccount = admins[ply:GetAdminAccount()]

		while players.GetByID(k) ~= nil do
			local plytarg = players.GetByID(k)

			if plytarg.IsAdmin then
				plytarg:SendChat( "^7[^4Admins^7] ^5" .. ouraccount["username"] .. "^7: " .. message )
			end

			k = k + 1
		end
		
	else
		AdmReply(ply, "^1You are not logged in.")
	end
end

local function Admin_Puppet(ply, argc, argv)
	-- EVIL... we can mimmick the actions of other people.
	-- Not for regular use, mostly for test purposes but some admins might find this humorous..

	if ply.IsAdmin then
		local rank = AdmRank_GetRank(ply)
		if rank["can-puppet"] ~= true then
			AdmReply(ply, "^1You do not have permission to perform this action.")
			return
		end

		if argc < 3 then
			AdmReply(ply, "^3Syntax: /admpuppet <player> <channel> <message> -OR-")
			AdmReply(ply, "/admpuppet <player> <message>")
			return
		end

		local plytarg = players.GetByArg(argv[1])
		if plytarg == nil then
			AdmReply(ply, "^1Invalid player specified.")
			return
		end

		local channeltarg = argv[2]
		local targetchan = 0

		if channeltarg == "global" then
			targetchan = 1
		elseif channeltarg == "yell" then
			targetchan = 2
		elseif channeltarg == "emote" then
			targetchan = 3
		elseif channeltarg == "me" then
			targetchan = 3
		elseif channeltarg == "action" then
			targetchan = 3
		elseif channeltarg == "team" then
			targetchan = 4
		end
		
		local message
		if targetchan == 0 then
			message = table.concat(argv," ",2, argc-1)
		else
			message = table.concat(argv," ",3, argc-1)
		end

		if targetchan == 0 then
			plytarg:ExecuteChatCommand("say " .. message)
		elseif targetchan == 1 then
			plytarg:ExecuteChatCommand("sayglobal " .. message)
		elseif targetchan == 2 then
			plytarg:ExecuteChatCommand("sayyell " .. message)
		elseif targetchan == 3 then
			plytarg:ExecuteChatCommand("sayact " .. message)
		elseif targetchan == 4 then
			plytarg:ExecuteChatCommand("say_team " .. message)
		else
			AdmReply(ply, "^1ERROR: This wasn't supposed to happen...abort, abort!")
			return
		end
	else
		AdmReply(ply, "^1You are not logged in.")
	end
end

--[[ ------------------------------------------------
	InitAdminCmds
	This is what links all the commands into chat/console.
--------------------------------------------------]]

local function InitAdminCmds()
	chatcmds.Add("admlogin", Admin_Login)
	chatcmds.Add("admlogout", Admin_Logout)
	chatcmds.Add("admkick", Admin_Kick)
	chatcmds.Add("admchangedetails", Admin_ChangeDetails)
	chatcmds.Add("admprofile", Admin_Profile)
	chatcmds.Add("admnewaccount", Admin_AddAccount)
	chatcmds.Add("admdeleteaccount", Admin_DeleteAccount)
	chatcmds.Add("admlist", Admin_List)
	chatcmds.Add("admrank", Admin_Rank)
	chatcmds.Add("admalter", Admin_Alter)
	chatcmds.Add("admstatus", Admin_Status)
	chatcmds.Add("admsay", Admin_Say)
	chatcmds.Add("admtell", Admin_Tell)
	chatcmds.Add("admspeak", Admin_Speak)
	chatcmds.Add("admpuppet", Admin_Puppet)

	-- For replying to /admtells..this isn't an admin command, it can be used by any client
	chatcmds.Add("admreply", Admin_Reply)

	-- Add them to the console too.
	cmds.Add("admlogin", Admin_Login)
	cmds.Add("admlogout", Admin_Logout)
	cmds.Add("admkick", Admin_Kick)
	cmds.Add("admchangedetails", Admin_ChangeDetails)
	cmds.Add("admprofile", Admin_Profile)
	cmds.Add("admnewaccount", Admin_AddAccount)
	cmds.Add("admdeleteaccount", Admin_DeleteAccount)
	cmds.Add("admlist", Admin_List)
	cmds.Add("admrank", Admin_Rank)
	cmds.Add("admalter", Admin_Alter)
	cmds.Add("admstatus", Admin_Status)
	cmds.Add("admsay", Admin_Say)
	cmds.Add("admtell", Admin_Tell)
	cmds.Add("admspeak", Admin_Speak)
	cmds.Add("admpuppet", Admin_Puppet)

	-- For replying to /admtells..this isn't an admin command, it can be used by any client
	cmds.Add("admreply", Admin_Reply)
end

--[[ ------------------------------------------------
	Init
--------------------------------------------------]]

local function InitAdminSys()
	InitAdminAccounts()
	InitAdminCmds()
end

InitAdminSys()