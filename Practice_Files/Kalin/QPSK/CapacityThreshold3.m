clear, close all;
load gc_eve1.mat;
load gc_eve2.mat;
load gc_eve3.mat;
load gc_eve4.mat;
load gc_bob.mat;
load gc_hall.mat;
load SNR.mat;

tic;

numSNRpoints = 300;

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
for sweep = 1:numSNRpoints
    if (Cs_evemin(sweep) == max(Cs_evemin))
        disp('index of best secrecy capacity is ' + string(sweep));
    end
end

Cs_e1(:,:) = sum(Cs_eve1(185,:,:,:));
Cs_e2(:,:) = sum(Cs_eve2(185,:,:,:));
Cs_e3(:,:) = sum(Cs_eve3(185,:,:,:));
Cs_e4(:,:) = sum(Cs_eve4(185,:,:,:));
Cs_b(:,:) = sum(Cs_bob(185,:,:,:));
Cs_h(:,:) = sum(Cs_hall(185,:,:,:));

Cs_threshold = nan(90, 345);
Cs_threshold(1:65, 1:61) = Cs_e1(:,:);
Cs_threshold(1:65, 123:148) = Cs_e2(:,:);
Cs_threshold(1:65, 154:179) = Cs_e3(:,:);
Cs_threshold(1:65, 213:240) = Cs_e4(:,:);
Cs_threshold(1:65, 65:98) = Cs_b(:,:);
Cs_threshold(66:90, :) = Cs_h(:,:);
save Cs_threshold.mat Cs_threshold;

toc;