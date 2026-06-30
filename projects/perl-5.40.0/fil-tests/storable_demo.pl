#!/usr/bin/env perl
use strict;
use warnings;
use Storable qw(store retrieve freeze thaw dclone);
use Data::Dumper;

# Create a complex data structure
my $data = {
    name => "John Doe",
    age => 30,
    hobbies => ["reading", "coding", "gaming"],
    address => {
        street => "123 Main St",
        city => "Anytown",
        zip => "12345"
    },
    scores => [95, 87, 92, 88, 91]
};

print "Original data structure:\n";
print Dumper($data);

# Store to file
print "\nStoring data to file 'data.storable'...\n";
store($data, 'data.storable');

# Retrieve from file
print "Retrieving data from file...\n";
my $retrieved_data = retrieve('data.storable');
print "Retrieved data:\n";
print Dumper($retrieved_data);

# Freeze to memory (serialize to scalar)
print "\nFreezing data to memory...\n";
my $frozen = freeze($data);
print "Frozen data size: " . length($frozen) . " bytes\n";

# Thaw from memory (deserialize from scalar)
print "Thawing data from memory...\n";
my $thawed_data = thaw($frozen);
print "Thawed data:\n";
print Dumper($thawed_data);

# Deep clone using dclone
print "\nCreating deep clone...\n";
my $cloned_data = dclone($data);
$cloned_data->{name} = "Jane Doe";
$cloned_data->{hobbies}->[0] = "writing";

print "Original data (unchanged):\n";
print "Name: $data->{name}, First hobby: $data->{hobbies}->[0]\n";
print "Cloned data (modified):\n";
print "Name: $cloned_data->{name}, First hobby: $cloned_data->{hobbies}->[0]\n";

# Clean up
unlink 'data.storable' if -e 'data.storable';
print "\nCleaned up temporary file.\n";