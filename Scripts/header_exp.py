import csv

def process_csv(csv_file):
    with open(csv_file, 'r') as f:
        reader = csv.reader(f)
        # convert the reader to a list
        data = list(reader)
        # get the header
        header = data[0]
        # remove the header from the data
        data = data[1:]
        # return the header and data
        return header, data

def export_to_header(filename, data):
    with open(filename, 'w') as f:
        f.write("/**\n * @author: Quinn Henthorne henth013@d.umn.edu 7636568391\n*/\n\n")
        
        for value in data:
            message = "#define {} {} //{}\n".format(value[6], value[0][4:], value[7])
            if(value[6] == '' or value[0] == ''):
                f.write('\n')
                continue
            f.write(message)
    

csv_path = '/home/quinn/Documents/Github/Contact-Dummy/Scripts/Pinout.csv'
header, data = process_csv(csv_path)
export_to_header('PINOUT.h', data)
