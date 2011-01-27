require 'exiv2'

describe Exiv2 do
  it "should read IPTC data" do
    image = Exiv2::ImageFactory.open("spec/files/test.jpg")
    image.read_metadata
    iptc_data = image.iptc_data
    iptc_data.should be_a(Exiv2::IptcData)
    iptc_data.to_a.should == [
      ["Iptc.Application2.Caption", "Rhubarb rhubarb rhubard"],
      ["Iptc.Application2.Keywords", "fish"],
      ["Iptc.Application2.Keywords", "custard"]
    ]
  end

  it "should raise an error when trying to open a non-existant file" do
    expect {
      Exiv2::ImageFactory.open("tmp/no-such-file.jpg")
    }.should raise_error(Exiv2::BasicError)
  end
end
