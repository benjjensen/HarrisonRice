%%% PieceTogetherArray %%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% This file takes the pwelch data arrays and inserts them in the proper
% locations with respect to one another into one array that represents the
% complete area over which the data was collected
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% First Transmitter Location
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

complete_pwelch = nan(128,90,345);

for temp = 1:65
    for loops = 1:61
        obj = 'conference_pwelch(:,' + string(temp) + ',' + string(loops) + ')';
        complete_pwelch(:,temp,loops) = eval(obj);
    end
end

for temp = 1:29
    for loops = 1:33
        obj = 'harrison_pwelch(:,' + string(temp) + ',' + string(loops) + ')';
        complete_pwelch(:,36+temp,64+loops) = eval(obj);
    end
end

for temp = 1:31
    for loops = 1:26
        obj = 'smalley_pwelch(:,' + string(temp) + ',' + string(loops) + ')';
        complete_pwelch(:,34+temp,122+loops) = eval(obj);
    end
end

for temp = 1:32
    for loops = 1:26
        obj = 'camacho_pwelch(:,' + string(temp) + ',' + string(loops) + ')';
        complete_pwelch(:,33+temp,153+loops) = eval(obj);
    end
end

for temp = 1:32
    for loops = 1:28
        obj = 'chambers_pwelch(:,' + string(temp) + ',' + string(loops) + ')';
        complete_pwelch(:,33+temp,212+loops) = eval(obj);
    end
end

for temp = 1:23
    for loops = 1:300
        obj = 'hallway_pwelch(:,' + string(temp) + ',' + string(loops) + ')';
        complete_pwelch(:,68+temp,42+loops) = eval(obj);
    end
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Second Transmitter Location
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

tx2_pwelch = nan(128,90,345);

for temp = 1:65
    for loops = 1:61
        obj = 'tx2conference_pwelch(:,' + string(temp) + ',' + string(loops) + ')';
        tx2_pwelch(:,temp,loops) = eval(obj);
    end
end

for temp = 1:30
    for loops = 1:34
        obj = 'tx2harrison_pwelch(:,' + string(temp) + ',' + string(loops) + ')';
        tx2_pwelch(:,35+temp,64+loops) = eval(obj);
    end
end

for temp = 1:32
    for loops = 1:26
        obj = 'tx2smalley_pwelch(:,' + string(temp) + ',' + string(loops) + ')';
        tx2_pwelch(:,33+temp,122+loops) = eval(obj);
    end
end

for temp = 1:32
    for loops = 1:26
        obj = 'tx2camacho_pwelch(:,' + string(temp) + ',' + string(loops) + ')';
        tx2_pwelch(:,33+temp,153+loops) = eval(obj);
    end
end

for temp = 1:32
    for loops = 1:28
        obj = 'tx2chambers_pwelch(:,' + string(temp) + ',' + string(loops) + ')';
        tx2_pwelch(:,33+temp,212+loops) = eval(obj);
    end
end

for temp = 1:23
    for loops = 1:300
        obj = 'tx2hallway_pwelch(:,' + string(temp) + ',' + string(loops) + ')';
        tx2_pwelch(:,68+temp,42+loops) = eval(obj);
    end
end

save('tx2_pwelch.mat','tx2_pwelch');