%%% CreateDataArray %%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% This file takes the individual arrays generated from data
% collection and first turns each section or room into a complete
% three dimensional array that is oriented such that it can be used
% for the generation of a heatmap (co-ordinate (1,1) is in the top
% left corner of the array). After each section, or room, has been
% oriented and combined, those arrays are then inserted into one large
% array that represents the entire section where our data was
% collected. Points where we do not have data are represented by NaN's
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% First Transmitter Location
% Desired format is (area)_data(2048,y-axis,x-axis)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Hallway
numbarrays = 300;
numbloops = 23;
hallway_data = nan(2048,numbloops,numbarrays);

for numbtests = 1:numbarrays
    for numbcorrections = 1:numbloops
        obj = 'hallway_' + string(numbtests) + '(:,' + string(numbcorrections) + ')';
        hallway_data(:,24 - numbcorrections,numbtests) = eval(obj);
    end
end

% Camacho's Office
numbarrays = 26;
numbloops = 32;
camacho_data = nan(2048,numbloops,numbarrays);

for numbtests = 1:numbarrays
    for numbcorrections = 1:numbloops
        obj = 'camacho_' + string(numbtests) + '(:,' + string(numbcorrections) + ')';
        camacho_data(:,33 - numbcorrections,numbtests) = eval(obj);
    end
end

% Chambers' Office
num_data_arrays = 28;
num_loops_for_area = 32;
chambers_data = nan(2048,num_loops_for_area,num_data_arrays);

for numtests = 1:num_data_arrays - 2
    obj = 'chambers_' + string(numtests) + '(:,:)';
    chambers_data(:,:,numtests) = eval(obj);
end

for numtests = num_data_arrays - 1:num_data_arrays
    obj = 'chambers_' + string(numtests) + '(:,:)';
    chambers_data(:,1:29,numtests) = eval(obj);
end

% Conference Room
numbarrays = 65;
numbloops = 61;
conference_data = nan(2048,numbarrays,numbloops);

for numbtests = 1:4
    for numbcorrections = 1:29
        obj = 'conference_south_wall_' + string(numbtests) + '(:,' + string(numbcorrections) + ')';
        conference_data(:,5 - numbtests,62 - numbcorrections) = eval(obj);
    end
end

for numbtests = 1:3
    for numbcorrections = 1:58
        obj = 'conference_south_pillar_' + string(numbtests) + '(:,' + string(numbcorrections) + ')';
        conference_data(:,8 - numbtests,62 - numbcorrections) = eval(obj);
    end
end

for numbtests = 1:55
    for numbcorrections = 1:61
        obj = 'conference_main_' + string(numbtests) + '(:,' + string(numbcorrections) + ')';
        conference_data(:,63 - numbtests,62 - numbcorrections) = eval(obj);
    end
end

for numbtests = 1:3
    for numbcorrections = 1:55
        obj = 'conference_north_' + string(numbtests) + '(:,' + string(numbcorrections) + ')';
        conference_data(:,66 - numbtests,62 - numbcorrections) = eval(obj);
    end
end

% Harrison's office
numbarrays = 29;
numbloops = 33;
harrison_data = nan(2048,numbarrays,numbloops);

for numbtests = 1:numbarrays
    obj = 'harrison_' + string(numbtests) + '(:,:)';
    harrison_data(:,30 - numbtests,:) = eval(obj);
end

% Smalley's office
numbarrays = 31;
numbloops = 26;
smalley_data = nan(2048,numbarrays,numbloops);

for numbtests = 1:numbarrays
    for numbcorrections = 1:numbloops
        obj = 'smalley_' + string(numbtests) + '(:,' + string(numbcorrections) + ')';
        smalley_data(:,32 - numbtests,27 - numbcorrections) = eval(obj);
    end
end

%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%
% Second Transmitter Location
% Desired format is (area)_data(2048,y-axis,x-axis)
%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%

% Hallway
numbarrays = 23;
numbloops = 300;
tx2hallway_data = nan(2048,numbarrays,numbloops);

for numbtests = 1:numbarrays
    obj = 'tx2hallway_' + string(numbtests) + '(:,:)';
    tx2hallway_data(:,numbtests,:) = eval(obj);
end

% Camacho's office
numbarrays = 26;
numbloops = 32;
tx2camacho_data = nan(2048,numbloops,numbarrays);

for numbtests = 1:numbarrays
    obj = 'tx2camacho_' + string(numbtests) + '(:,:)';
    tx2camacho_data(:,:,numbtests) = eval(obj);
end

% Chambers' office
numbarrays = 28;
numbloops = 32;
tx2chambers_data = nan(2048,numbloops,numbarrays);

for numbtests = 1:numbarrays - 2
    obj = 'tx2chambers_' + string(numbtests) + '(:,:)';
    tx2chambers_data(:,:,numbtests) = eval(obj);
end

for numbtests = numbarrays - 1:numbarrays
    obj = 'tx2chambers_' + string(numbtests) + '(:,:)';
    tx2chambers_data(:,1:29,numbtests) = eval(obj);
end

% Conference Room
numbarrays = 65;
numbloops = 61;
tx2conference_data = nan(2048,numbarrays,numbloops);

for numbtests = 1:4
    for numbcorrections = 1:30
        obj = 'tx2conference_' + string(numbtests) + '(:,' + string(numbcorrections) + ')';
        tx2conference_data(:,numbtests,31 + numbcorrections) = eval(obj);
    end
end

for numbtests = 5:7
    for numbcorrections = 1:58
        obj = 'tx2conference_' + string(numbtests) + '(:,' + string(numbcorrections) + ')';
        tx2conference_data(:,numbtests,3 + numbcorrections) = eval(obj);
    end
end

for numbtests = 8:62
    for numbcorrections = 1:61
        obj = 'tx2conference_' + string(numbtests) + '(:,' + string(numbcorrections) + ')';
        tx2conference_data(:,numbtests,numbcorrections) = eval(obj);
    end
end

for numbtests = 63:65
    for numbcorrections = 1:55
        obj = 'tx2conference_' + string(numbtests) + '(:,' + string(numbcorrections) + ')';
        tx2conference_data(:,numbtests,3 + numbcorrections) = eval(obj);
    end
end

% Harrison's office
numbarrays = 30;
numbloops = 34;
tx2harrison_data = nan(2048,numbarrays,numbloops);

for numbtests = 1:numbarrays
    obj = 'tx2harrison_' + string(numbtests) + '(:,:)';
    tx2harrison_data(:,numbtests,:) = eval(obj);
end

% Smalley's office
numbarrays = 32;
numbloops = 26;
tx2smalley_data = nan(2048,numbarrays,numbloops);

for numbtests = 1:numbarrays
    obj = 'tx2smalley_' + string(numbtests) + '(:,:)';
    tx2smalley_data(:,numbtests,:) = eval(obj);
end

% Call file PwelchData
PwelchData;

% Call file PieceTogetherArray
PieceTogetherArray;

% Call file SeparateSignals
SeparateSignals;
