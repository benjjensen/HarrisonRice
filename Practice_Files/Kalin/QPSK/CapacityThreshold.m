clear; 

tic;

load linear_signal.mat;
load linear_noisefloor.mat;
for y = 1:90
    for z = 1:345
        linear_signal(:,y,z) = fftshift(linear_signal(:,y,z));
    end
end

noise = linear_noisefloor(10:54,:,:);
for y = 1:90
    for z = 1:345
        noise(:,y,z) = sum(noise(:,y,z)) ./ 45;
    end
end

gc_signal = linear_signal(11:55,:,:);
gc_signal = gc_signal ./ noise;
g = sqrt(gc_signal);
for y = 1:90
    for z = 1:345
        g_norm(:,y,z) = g(:,y,z) ./ max(g(:,y,z));
        g_squared(:,y,z) = g_norm(:,y,z) .^ 2;
    end
end

% SNRdB = linspace(-20, 20, 1000);
% SNR = 10 .^ (SNRdB / 10);
numSNRpoints = 300;
SNR_lin = logspace(-4, 7, numSNRpoints);
SNR = 10*log10(SNR_lin);

tau = 10^(5/10); % This should be between -10 and 10 dB, the value chosen here is 5 dB
parfor sweep = 1:numSNRpoints
    % Bobs room is found at g_squared(:,36:65,65:98)
    disp(string(sweep) + ' / ' + string(numSNRpoints) + ' g*snr array');
    for y = 1:90
        for z = 1:345
            for x = 1:45
                if isnan(g_squared(x,y,z))
                    g_times_snr(sweep, x, y, z) = nan;
                else
                    g_times_snr(sweep, x, y, z) = g_squared(x,y,z) * SNR_lin(sweep);
                end
            end
        end
    end
end
disp('done with g_times_snr');

result_eve1 = g_times_snr(:,:,1:65,1:61);
result_eve2 = g_times_snr(:,:,1:65,123:148);
result_eve3 = g_times_snr(:,:,1:65,154:179);
result_eve4 = g_times_snr(:,:,1:65,213:240);
result_bob = g_times_snr(:,:,1:65,65:98);
result_hall = g_times_snr(:,:,66:90,:);

parfor sweep = 1:numSNRpoints
    disp(string(sweep) + ' / ' + string(numSNRpoints) + ' separate into rooms and see if above threshold part 1/4');
    for x = 1:45
        for y = 1:65
            for z = 1:61
                if isnan(result_eve1(sweep,x,y,z))
                    gc_eve1(sweep,x,y,z) = nan;
                elseif result_eve1(sweep,x,y,z) >= tau
                    gc_eve1(sweep,x,y,z) = 1;
                else
                    gc_eve1(sweep,x,y,z) = 0;
                end
            end
        end
    end
end

parfor sweep = 1:numSNRpoints
    disp(string(sweep) + ' / ' + string(numSNRpoints) + ' separate into rooms and see if above threshold part 2/4');
    for x = 1:45
        for y = 1:65
            for z = 1:26
                if isnan(result_eve2(sweep,x,y,z))
                    gc_eve2(sweep,x,y,z) = nan;
                elseif result_eve2(sweep,x,y,z) >= tau
                    gc_eve2(sweep,x,y,z) = 1;
                else
                    gc_eve2(sweep,x,y,z) = 0;
                end
                if isnan(result_eve3(sweep,x,y,z))
                    gc_eve3(sweep,x,y,z) = nan;
                elseif result_eve3(sweep,x,y,z) >= tau
                    gc_eve3(sweep,x,y,z) = 1;
                else
                    gc_eve3(sweep,x,y,z) = 0;
                end
            end
        end
    end
end

parfor sweep = 1:numSNRpoints
    disp(string(sweep) + ' / ' + string(numSNRpoints) + ' separate into rooms and see if above threshold part 3/4');
    for x = 1:45
        for y = 1:65
            for z = 1:28
                if isnan(result_eve4(sweep,x,y,z))
                    gc_eve4(sweep,x,y,z) = nan;
                elseif result_eve4(sweep,x,y,z) >= tau
                    gc_eve4(sweep,x,y,z) = 1;
                else
                    gc_eve4(sweep,x,y,z) = 0;
                end
            end
        end
    end
end

parfor sweep = 1:numSNRpoints
    disp(string(sweep) + ' / ' + string(numSNRpoints) + ' separate into rooms and see if above threshold part 4/4');
    for x = 1:45
        for y = 1:65
            for z = 1:34
                if isnan(result_bob(sweep,x,y,z))
                    gc_bob(sweep,x,y,z) = nan;
                elseif result_bob(sweep,x,y,z) >= tau
                    gc_bob(sweep,x,y,z) = 1;
                else
                    gc_bob(sweep,x,y,z) = 0;
                end
            end
        end
    end
end

parfor sweep = 1:numSNRpoints
    disp(string(sweep) + ' / ' + string(numSNRpoints) + ' separate into rooms and see if above threshold part 5/4');
    for x = 1:45
        for y = 1:25
            for z = 1:345
                if isnan(result_hall(sweep,x,y,z))
                    gc_hall(sweep,x,y,z) = nan;
                elseif result_hall(sweep,x,y,z) >= tau
                    gc_hall(sweep,x,y,z) = 1;
                else
                    gc_hall(sweep,x,y,z) = 0;
                end
            end
        end
    end
end
                        
bobcount(numSNRpoints) = zeros();
bobbest(numSNRpoints) = zeros();

for sweep = 1:numSNRpoints
    disp(string(sweep) + ' / ' + string(numSNRpoints) + ' get best carrier count for bob for each snr');
    for y = 1:65
        for z = 1:34
            if ~isnan(gc_bob(sweep,1,y,z))
                bobcount(sweep) = sum(gc_bob(sweep,:,y,z));
                if bobcount(sweep) > bobbest(sweep)
                    bobbest(sweep) = bobcount(sweep);
                    bob_carriers(sweep,:) = gc_bob(sweep,:,y,z);
                end
            end
        end
    end
end
eve1count(numSNRpoints) = zeros();
eve2count(numSNRpoints) = zeros();
eve3count(numSNRpoints) = zeros();
eve4count(numSNRpoints) = zeros();
eve1best(numSNRpoints) = zeros();
eve2best(numSNRpoints) = zeros();
eve3best(numSNRpoints) = zeros();
eve4best(numSNRpoints) = zeros();
for sweep = 1:numSNRpoints
    disp(string(sweep) + ' / ' + string(numSNRpoints) + ' get best carrier count for eve for each snr');    
    for y = 1:65
        for z = 1:61
            if ~isnan(gc_eve1(sweep,1,y,z))
                eve1count(sweep) = sum(gc_eve1(sweep,:,y,z));
                if eve1count(sweep) > eve1best(sweep)
                    eve1best(sweep) = eve1count(sweep);
                    eve1_carriers(sweep,:) = gc_eve1(sweep,:,y,z);
                end
            end
        end
        for z = 1:26
            if ~isnan(gc_eve2(sweep,1,y,z))
                eve2count(sweep) = sum(gc_eve2(sweep,:,y,z));
                if eve2count(sweep) > eve2best(sweep)
                    eve2best(sweep) = eve2count(sweep);
                    eve2_carriers(sweep,:) = gc_eve2(sweep,:,y,z);
                end
            end
            if ~isnan(gc_eve3(sweep,1,y,z))
                eve3count(sweep) = sum(gc_eve3(sweep,:,y,z));
                if eve3count(sweep) > eve3best(sweep)
                    eve3best(sweep) = eve3count(sweep);
                    eve3_carriers(sweep,:) = gc_eve3(sweep,:,y,z);
                end
            end
        end
        for z = 1:28
            if ~isnan(gc_eve4(sweep,1,y,z))
                eve4count(sweep) = sum(gc_eve4(sweep,:,y,z));
                if eve4count(sweep) > eve4best(sweep)
                    eve4best(sweep) = eve4count(sweep);
                    eve4_carriers(sweep,:) = gc_eve4(sweep,:,y,z);
                end
            end
        end
    end
end

for sweep = 1:numSNRpoints
    disp(string(sweep) + ' / ' + string(numSNRpoints) + ' find carriers eve shares with bob');    
    for carrier = 1:45
        if (bob_carriers(sweep, carrier) == 1) && (eve1_carriers(sweep, carrier) == 1)
            eve1_carriers_shared(sweep,carrier) = 1;
        else
            eve1_carriers_shared(sweep,carrier) = 0;
        end
        if (bob_carriers(sweep, carrier) == 1) && (eve2_carriers(sweep, carrier) == 1)
            eve2_carriers_shared(sweep,carrier) = 1;
        else
            eve2_carriers_shared(sweep,carrier) = 0;
        end
        if (bob_carriers(sweep, carrier) == 1) && (eve3_carriers(sweep, carrier) == 1)
            eve3_carriers_shared(sweep,carrier) = 1;
        else
            eve3_carriers_shared(sweep,carrier) = 0;
        end
        if (bob_carriers(sweep, carrier) == 1) && (eve4_carriers(sweep, carrier) == 1)
            eve4_carriers_shared(sweep,carrier) = 1;
        else
            eve4_carriers_shared(sweep,carrier) = 0;
        end
    end
end

% Plot Best Capacity for Bob and Eve
for sweep = 1:numSNRpoints
    evebest(sweep) = max(eve1best(sweep), eve2best(sweep));
    evebest(sweep) = max(evebest(sweep), eve3best(sweep));
    evebest(sweep) = max(evebest(sweep), eve4best(sweep));
end
figure();
plot(SNR,bobbest);
hold on
plot(SNR,evebest);
hold off
legend({'bob', 'eve'});

disp('done with capacity plot');

% Find Location of Eve's best advantage
% maxdiff = 0;
% eveAtDiff = 0;
% indexAtDiff = 0;
% for sweep = 1:numSNRpoints
%     diff = evebest(sweep) - bobbest(sweep);
%     if diff > maxdiff
%         maxdiff = diff;
%         eveAtDiff = evebest(sweep);
%         indexAtDiff = sweep;
%     end
% end
% 
% if eve1best(indexAtDiff) == eveAtDiff
%     maxDiffIsEve1 = true;
% else
%     maxDiffisEve1 = false;
% end
% if eve2best(indexAtDiff) == eveAtDiff
%     maxDiffIsEve2 = true;
% else
%     maxDiffisEve2 = false;
% end
% if eve3best(indexAtDiff) == eveAtDiff
%     maxDiffIsEve3 = true; % It happens at y: 59, z: 1 and bobs y:57, z:28
% else
%     maxDiffisEve3 = false;
% end
% if eve4best(indexAtDiff) == eveAtDiff
%     maxDiffIsEve4 = true;
% else
%     maxDiffisEve4 = false;
% end

% Begin Secrecy Capacity calculations
Cs_eve1 = nan(numSNRpoints, 45, 65, 61);
Cs_eve2 = nan(numSNRpoints, 45, 65, 26);
Cs_eve3 = nan(numSNRpoints, 45, 65, 26);
Cs_eve4 = nan(numSNRpoints, 45, 65, 28);
Cs_bob = nan(numSNRpoints, 45, 65, 34);
Cs_hall = nan(numSNRpoints, 45, 25, 345);

parfor sweep = 1:numSNRpoints
    disp(string(sweep) + ' / ' + string(numSNRpoints) + ' Cs array eve1');    
    for y = 1:65
        for z = 1:61
            for carrier = 1:45
                if ~isnan(gc_eve1(sweep, carrier, y, z))
                    if (bob_carriers(sweep, carrier) - gc_eve1(sweep, carrier, y, z)) < 0
                        Cs_eve1(sweep, carrier, y, z) = 0;
                    else
                        Cs_eve1(sweep, carrier, y, z) = bob_carriers(sweep, carrier) - gc_eve1(sweep, carrier, y, z);
                    end
                end
            end
        end
    end
end

parfor sweep = 1:numSNRpoints
    disp(string(sweep) + ' / ' + string(numSNRpoints) + ' Cs array eve2&3');
    for y = 1:65
        for z = 1:26
            for carrier = 1:45
                if ~isnan(gc_eve2(sweep, carrier, y, z))
                    if (bob_carriers(sweep, carrier) - gc_eve2(sweep, carrier, y, z)) < 0
                        Cs_eve2(sweep, carrier, y, z) = 0;
                    else
                        Cs_eve2(sweep, carrier, y, z) = bob_carriers(sweep, carrier) - gc_eve2(sweep, carrier, y, z);
                    end
                end
                if ~isnan(gc_eve3(sweep, carrier, y, z))
                    if (bob_carriers(sweep, carrier) - gc_eve3(sweep, carrier, y, z)) < 0
                        Cs_eve3(sweep, carrier, y, z) = 0;
                    else
                        Cs_eve3(sweep, carrier, y, z) = bob_carriers(sweep, carrier) - gc_eve3(sweep, carrier, y, z);
                    end
                end
            end
        end
    end
end

parfor sweep = 1:numSNRpoints
    disp(string(sweep) + ' / ' + string(numSNRpoints) + ' Cs array eve4');
    for y = 1:65
        for z = 1:28
            for carrier = 1:45
                if ~isnan(gc_eve4(sweep, carrier, y, z))
                    if (bob_carriers(sweep, carrier) - gc_eve4(sweep, carrier, y, z)) < 0
                        Cs_eve4(sweep, carrier, y, z) = 0;
                    else
                        Cs_eve4(sweep, carrier, y, z) = bob_carriers(sweep, carrier) - gc_eve4(sweep, carrier, y, z);
                    end
                end
            end
        end
    end
end

parfor sweep = 1:numSNRpoints
    disp(string(sweep) + ' / ' + string(numSNRpoints) + ' Cs array bob');
    for y = 1:65
        for z = 1:34
            for carrier = 1:45
                if ~isnan(gc_bob(sweep, carrier, y, z))
                    if (bob_carriers(sweep, carrier) - gc_bob(sweep, carrier, y, z)) < 0
                        Cs_bob(sweep, carrier, y, z) = 0;
                    else
                        Cs_bob(sweep, carrier, y, z) = bob_carriers(sweep, carrier) - gc_bob(sweep, carrier, y, z);
                    end
                end
            end
        end
    end
end

parfor sweep = 1:numSNRpoints
    disp(string(sweep) + ' / ' + string(numSNRpoints) + ' Cs array bob');
    for y = 1:25
        for z = 1:345
            for carrier = 1:45
                if ~isnan(gc_hall(sweep, carrier, y, z))
                    if (bob_carriers(sweep, carrier) - gc_hall(sweep, carrier, y, z)) < 0
                        Cs_hall(sweep, carrier, y, z) = 0;
                    else
                        Cs_hall(sweep, carrier, y, z) = bob_carriers(sweep, carrier) - gc_hall(sweep, carrier, y, z);
                    end
                end
            end
        end
    end
end

Cs_eve1_min(numSNRpoints) = zeros();
Cs_eve1_min(:) = 45;
Cs_eve2_min(numSNRpoints) = zeros();
Cs_eve2_min(:) = 45;
Cs_eve3_min(numSNRpoints) = zeros();
Cs_eve3_min(:) = 45;
Cs_eve4_min(numSNRpoints) = zeros();
Cs_eve4_min(:) = 45;
parfor sweep = 1:numSNRpoints
    disp(string(sweep) + ' / ' + string(numSNRpoints) + ' Cs min eve1');
    for y = 1:65
        for z = 1:61
            if sum(Cs_eve1(sweep,:,y,z)) < Cs_eve1_min(sweep)
                Cs_eve1_min(sweep) = sum(Cs_eve1(sweep,:,y,z));
            end
        end
    end
end
    
parfor sweep = 1:numSNRpoints
    disp(string(sweep) + ' / ' + string(numSNRpoints) + ' Cs min eve2&3');
    for y = 1:65
        for z = 1:26
            if sum(Cs_eve2(sweep,:,y,z)) < Cs_eve2_min(sweep)
                Cs_eve2_min(sweep) = sum(Cs_eve2(sweep,:,y,z));
            end
            if sum(Cs_eve3(sweep,:,y,z)) < Cs_eve3_min(sweep)
                Cs_eve3_min(sweep) = sum(Cs_eve3(sweep,:,y,z));
            end
        end
    end
end

parfor sweep = 1:numSNRpoints
    disp(string(sweep) + ' / ' + string(numSNRpoints) + ' Cs min eve4');
    for y = 1:65
        for z = 1:28
            if sum(Cs_eve4(sweep,:,y,z)) < Cs_eve4_min(sweep)
                Cs_eve4_min(sweep) = sum(Cs_eve4(sweep,:,y,z));
            end
        end
    end
end

for sweep = 1:numSNRpoints
    Cs_evemin(sweep) = min(Cs_eve1_min(sweep), Cs_eve2_min(sweep));
    Cs_evemin(sweep) = min(Cs_evemin(sweep), Cs_eve3_min(sweep));
    Cs_evemin(sweep) = min(Cs_evemin(sweep), Cs_eve4_min(sweep));
end

figure();
plot(SNR, Cs_evemin);

% Save an array for Dr. Harrison
threshold_capacity_data(1,:) = SNR;
threshold_capacity_data(2,:) = bobbest;
threshold_capacity_data(3,:) = evebest;
threshold_capacity_data(4,:) = Cs_evemin;
% save threshold_capacity_data.mat threshold_capacity_data;

% Organize into proper format for a heatmap
Cs_index = 0;
for sweep = 1:numSNRpoints
    if (Cs_evemin(sweep) == max(Cs_evemin))
        disp('index of best secrecy capacity is ' + string(sweep));
        Cs_index = sweep;
    end
end

Cs_e1(:,:) = sum(Cs_eve1(sweep,:,:,:));
Cs_e2(:,:) = sum(Cs_eve2(sweep,:,:,:));
Cs_e3(:,:) = sum(Cs_eve3(sweep,:,:,:));
Cs_e4(:,:) = sum(Cs_eve4(sweep,:,:,:));
Cs_b(:,:) = sum(Cs_bob(sweep,:,:,:));
Cs_h(:,:) = sum(Cs_hall(sweep,:,:,:));

Cs_threshold = nan(90, 345);
Cs_threshold(1:65, 1:61) = Cs_e1(:,:);
Cs_threshold(1:65, 123:148) = Cs_e2(:,:);
Cs_threshold(1:65, 154:179) = Cs_e3(:,:);
Cs_threshold(1:65, 213:240) = Cs_e4(:,:);
Cs_threshold(1:65, 65:98) = Cs_b(:,:);
Cs_threshold(66:90, :) = Cs_h(:,:);
save Cs_threshold.mat Cs_threshold;

toc;