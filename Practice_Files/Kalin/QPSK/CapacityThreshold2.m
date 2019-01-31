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

% Find Location of Eve's best advantage
maxdiff = 0;
eveAtDiff = 0;
indexAtDiff = 0;
for sweep = 1:numSNRpoints
    diff = evebest(sweep) - bobbest(sweep);
    if diff > maxdiff
        maxdiff = diff;
        eveAtDiff = evebest(sweep);
        indexAtDiff = sweep;
    end
end

if eve1best(indexAtDiff) == eveAtDiff
    maxDiffIsEve1 = true;
else
    maxDiffisEve1 = false;
end
if eve2best(indexAtDiff) == eveAtDiff
    maxDiffIsEve2 = true;
else
    maxDiffisEve2 = false;
end
if eve3best(indexAtDiff) == eveAtDiff
    maxDiffIsEve3 = true; % It happens at y: 59, z: 1 and bobs y:57, z:28
else
    maxDiffisEve3 = false;
end
if eve4best(indexAtDiff) == eveAtDiff
    maxDiffIsEve4 = true;
else
    maxDiffisEve4 = false;
end

toc;