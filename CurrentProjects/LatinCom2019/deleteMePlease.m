mfw = dsp.MatFileWriter("DeleteMePlease.mat");

temp1 = ones(2048, 5000);

for index = 1:1000
    temp2 = temp1(:,index);
    mfw(temp2);
end

%%

temp2 = temp1(2000, 5);
mfw(temp2);