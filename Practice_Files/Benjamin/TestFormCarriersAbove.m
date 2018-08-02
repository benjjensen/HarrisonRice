%%% CreateDataArray

% load all the data

num = 3000;  %size

harrisonArray = createArray(num, harrisons);        %% update with data name
smalleyArray = createArray(num, smalleys);          %% update with data name
camachoArray = createArray(num, camachos);          %% ...
chambersArray = createArray(num, chambers);
conferenceArray = createArray(num, conference);
    

%%% PwelchData %%%

Nfft = 2*64;
FF = -0.5:1/Nfft:0.5-1/Nfft;
FF = 20*FF;

harrisonPwelch = pwelchData(harrisonArray); %%% May need to pass in FF, Nfft
smalleyPwelch = pwelchData(smalleyArray);
camachoPwelch = pwelchData(camachoArray);
chambersPwelch = pwelchData(chambersArray);
conferencePwelch = pwelchData(conferenceArray);


%%% Piece together Array - Necessary?

%%% SeparateSignals %%%

dblimit = 20;
% save('dblimit.mat','dblimit');

harrisonCarriersAbove = separateSignals(harrisonPwelch);
smalleyCarriersAbove = separateSignals(smalleyPwelch);


numbarrays = 26;
numbloops = 32;
tx2camacho_data = nan(2048,numbloops,numbarrays);

function [data] = createArray(num, room)
    data = nan(2048,num);

    for i = 1:num
        eval(sprintf('data(:,num + 1 - %d) = (room)(:,%d)',i,i));     %%Update with room
    end

%     for numCorrections = 1:num    
%        obj = 'hallway_' + string(numCorrections) + '(:,' + string(numbcorrections) + ')';
%        data(:,numLoops + 1 - numbcorrections,numCorrections) = eval(obj);
%     end
end

function [pwelch_] = pwelchData(data)
   
    clear nanarray;
    nanarray = isnan(data);
    [~, num] = size(data);
    pwelch_ = nan(128,num);      % Pwelch drops the size 

    for j = 1:num
        if nanarray(:,j) ~= 1
            YY = pwelch(data(:,j),boxcar(Nfft),0,Nfft,'twosided');
            pwelch_(:,j) = 10*log10(abs(fftshift(YY)));
        end
    end

    %obj = sprintf("'%s_pwelch.mat','pwelch'",name);
    %save(obj);  
    % clear...
end

function [carriersAbove] = separateSignals(data)

    signalArray = nan(64, num);            %%%  64 b/c carriers
    noiseArray = nan(64, num);
    differenceArray = nan(64, num);
    carriersAbove = nan(num);

    for temp = 1:num
        signalArray(:,temp) = data(1:2:end,temp);
        noiseArray(:,temp) = data(2:2:end,temp);
        if isnan(signalArray(:,temp))
            differenceArray(:,temp) = nan;
        else
            differenceArray(:,temp) = abs(signalArray(:,temp) - noiseArray(:,temp));
        end

        count = 0;
        for carriers = 1:64
            if differenceArray(carriers,temp) >= dblimit
                count = count + 1;
            end
        end

        if isnan(differenceArray(:,temp))
            carriersAbove(temp) = nan;
        else
            carriersAbove(temp) = count;
        end
    end

    clear signalArray;
    clear noiseArray;
    clear differenceArray;

end
