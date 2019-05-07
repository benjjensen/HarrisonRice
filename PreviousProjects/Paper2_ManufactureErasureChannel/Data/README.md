Description of the Data found in PreviousProjects\Paper2...\Data:

General Information:

    - When working with data, we generally ignored the edge carriers and only used 10-55 (?)
    - The boundaries for each office are as follows:
        Conference: 5:65, 1:61
        Harrisons: 36:65, 65:98
        Smalley: 34:65, 123:148
        Camacho: 34:65, 154:179
        Chambers: 34:65, 213:240

completeDataMatrix: 

       The data matrix used to generate Fig. 6 in Paper 2. This matrix is 10x200 
    and is a combination of the capacities and secrecy capacities for the gaussian,
    cross, and threshold methods. 
    The organization is as follows:
        Row 1: SNR values 
        Rows 2-4: Bob's capacity, Eve's capacity, and secrecy capacity, respectively,
                     for the gaussian model.
                   (Rows 1-4 were generated in gaussian_case.m)
        Rows 5-7: Bob's cap, Eve's cap, and sec cap, respectively, for the threshold 
                     model.
                   (Rows 5-7 were generated in threshold_method.m)
        Rows 8-10: Bob's cap, Eve's cap, and sec cap for the cross model.
                   (Rows 8-10 were generated in cross_method.m, which takes 2-3 hours).

    All data was gathered based off of an SNR logspace from -3 to 4, and 200 loops


