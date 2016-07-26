# coding: utf-8
require 'bundler/setup'
require 'exiv2'
require 'fileutils'

describe Exiv2 do

  it "should handle a Pathname being passed to open" do
    image = Exiv2::ImageFactory.open(Pathname.new("spec/files/test.jpg").to_s)
    image.read_metadata
    expect(image.iptc_data.to_hash).not_to be_empty
  end

  it "should raise an error when trying to open a non-existant file" do
    expect {
      Exiv2::ImageFactory.open("tmp/no-such-file.jpg")
    }.to raise_error(Exiv2::BasicError)
  end

  it "should write metadata" do
    FileUtils.cp("spec/files/test.jpg", "spec/files/test_tmp.jpg")
    image = Exiv2::ImageFactory.open("spec/files/test_tmp.jpg")
    image.read_metadata
    image.iptc_data["Iptc.Application2.Caption"] = "A New Caption"
    image.write_metadata
    image = nil
    
    image2 = Exiv2::ImageFactory.open("spec/files/test_tmp.jpg")
    image2.read_metadata
    expect(image2.iptc_data["Iptc.Application2.Caption"]).to eq("A New Caption")
    FileUtils.rm("spec/files/test_tmp.jpg")
  end

  it 'reads UTF-8 data' do
    image = Exiv2::ImageFactory.open(Pathname.new("spec/files/photo_with_utf8_description.jpg").to_s)
    image.read_metadata
    description = image.exif_data["Exif.Image.ImageDescription"]
    if description.respond_to? :encoding # Only in Ruby 1.9+
      expect(description.encoding).to eq(Encoding::UTF_8)
    end
    expect(description).to eq('UTF-8 description. ☃ł㌎')
  end

  it 'reads UTF-8 data in each' do
    if String.new.respond_to? :encoding # Only in Ruby 1.9+
      image = Exiv2::ImageFactory.open(Pathname.new("spec/files/photo_with_utf8_description.jpg").to_s)
      image.read_metadata
      image.exif_data.each do |key,value|
        expect(key.encoding).to   eq(Encoding::UTF_8)
        expect(value.encoding).to eq(Encoding::UTF_8)
      end
    end
  end
  
  let(:image) do
    image = Exiv2::ImageFactory.open("spec/files/test.jpg")
    image.read_metadata
    image
  end

  context "IPTC data" do
    before do
      @iptc_data = image.iptc_data
    end

    it "should read IPTC data" do
      expect(@iptc_data).to be_a(Exiv2::IptcData)
      expect(@iptc_data.inspect).to eq('#<Exiv2::IptcData: {"Iptc.Application2.Caption"=>"Rhubarb rhubarb rhubard", "Iptc.Application2.Keywords"=>["fish", "custard"]}>')
      expect(@iptc_data.to_a).to eq([
        ["Iptc.Application2.Caption", "Rhubarb rhubarb rhubard"],
        ["Iptc.Application2.Keywords", "fish"],
        ["Iptc.Application2.Keywords", "custard"]
      ])
    end

    it "should convert iptc data into a hash" do
      iptc_hash = @iptc_data.to_hash
      expect(iptc_hash).to be_a(Hash)
      expect(iptc_hash).to eq({
        "Iptc.Application2.Caption"  => "Rhubarb rhubarb rhubard",
        "Iptc.Application2.Keywords" => ["fish", "custard"]
      })
    end
    
    it "should write IPTC data" do
      @iptc_data.add("Iptc.Application2.Keywords", "fishy")
      expect(@iptc_data.to_a).to eq([
        ["Iptc.Application2.Caption", "Rhubarb rhubarb rhubard"],
        ["Iptc.Application2.Keywords", "fish"],
        ["Iptc.Application2.Keywords", "custard"],
        ["Iptc.Application2.Keywords", "fishy"]
      ])
    end
    
    it "should set IPTC data" do
      @iptc_data["Iptc.Application2.Caption"] = "A New Caption"
      expect(@iptc_data.to_hash["Iptc.Application2.Caption"]).to eq("A New Caption")
    end
    
    it "should set multiply IPTC data values" do
      @iptc_data["Iptc.Application2.Keywords"] = ["abc", "cde"]
      expect(@iptc_data.to_hash["Iptc.Application2.Keywords"]).to eq(["abc", "cde"])
    end
    
    it "should delete one value of IPTC data" do
      @iptc_data.delete("Iptc.Application2.Keywords")
      expect(@iptc_data.to_hash["Iptc.Application2.Keywords"]).to eq("custard")
    end
    
    it "should delete all values of IPTC data" do
      @iptc_data.delete_all("Iptc.Application2.Keywords")
      expect(@iptc_data.to_hash["Iptc.Application2.Keywords"]).to eq(nil)
    end
  end

  context "XMP data" do
    before do
      @xmp_data = image.xmp_data
    end

    it "should read XMP data" do
      expect(@xmp_data).to be_a(Exiv2::XmpData)
      expect(@xmp_data.inspect).to eq('#<Exiv2::XmpData: {"Xmp.dc.description"=>"lang=\"x-default\" This is a description", "Xmp.dc.title"=>"lang=\"x-default\" Pickled"}>')
      expect(@xmp_data.to_a).to eq([
        ["Xmp.dc.title", "lang=\"x-default\" Pickled"],
        ["Xmp.dc.description", "lang=\"x-default\" This is a description"]
      ])
    end

    it "should convert xmp data into a hash" do
      xmp_hash = @xmp_data.to_hash
      expect(xmp_hash).to be_a(Hash)
      expect(xmp_hash).to eq({
        "Xmp.dc.title"       => "lang=\"x-default\" Pickled",
        "Xmp.dc.description" => "lang=\"x-default\" This is a description"
      })
    end

    it "should write XMP data" do
      @xmp_data["Xmp.dc.title"] = "lang=\"x-default\" Changed!"
      expect(@xmp_data.to_hash["Xmp.dc.title"]).to eq("lang=\"x-default\" Changed!")
    end
    
    it "should set XMP data" do
      @xmp_data["Xmp.dc.title"] = "A New Title"
      expect(@xmp_data.to_hash["Xmp.dc.title"]).to eq("lang=\"x-default\" A New Title")
    end
    
    it "should set multiply XMP data values" do
      @xmp_data["Xmp.dc.title"] = ["abc", "cde"]
      expect(@xmp_data.to_hash["Xmp.dc.title"]).to eq(["lang=\"x-default\" abc", "lang=\"x-default\" cde"])
    end
    
    it "should delete one value of XMP data" do
      @xmp_data["Xmp.dc.title"] = ["abc", "cde"]
      @xmp_data.delete("Xmp.dc.title")
      expect(@xmp_data.to_hash["Xmp.dc.title"]).to eq("lang=\"x-default\" cde")
    end
    
    it "should delete all values of XMP data" do
      @xmp_data.delete_all("Xmp.dc.title")
      expect(@xmp_data.to_hash["Xmp.dc.title"]).to eq(nil)
    end
  end

  context "EXIF data" do
    before do
      @exif_data = image.exif_data
    end

    it "should read Exif data" do
      expect(@exif_data).to be_a(Exiv2::ExifData)
      expect(@exif_data.inspect).to eq('#<Exiv2::ExifData: {"Exif.Image.ExifTag"=>"52", "Exif.Image.Software"=>"plasq skitch", "Exif.Photo.ExifVersion"=>"48 50 49 48", "Exif.Photo.PixelXDimension"=>"32", "Exif.Photo.PixelYDimension"=>"32"}>')
      expect(@exif_data.to_a).to eq([
        ["Exif.Image.Software",         "plasq skitch"],
        ["Exif.Image.ExifTag",          "52"],
        ["Exif.Photo.ExifVersion",      "48 50 49 48"],
        ["Exif.Photo.PixelXDimension",  "32"],
        ["Exif.Photo.PixelYDimension",  "32"]
      ])
    end

    it "should convert xmp data into a hash" do
      exif_hash = @exif_data.to_hash
      expect(exif_hash).to be_a(Hash)
      expect(exif_hash).to eq({
        "Exif.Photo.PixelXDimension" => "32",
        "Exif.Photo.ExifVersion"     => "48 50 49 48",
        "Exif.Image.Software"        => "plasq skitch",
        "Exif.Photo.PixelYDimension" => "32",
        "Exif.Image.ExifTag"         => "52"
      })
    end

    it "should write Exif data" do
      @exif_data.add("Exif.Image.Software", "ruby-exiv2")
      expect(@exif_data.to_hash).to eq({
        "Exif.Photo.PixelXDimension" => "32",
        "Exif.Photo.ExifVersion"     => "48 50 49 48",
        "Exif.Image.Software"        => ["plasq skitch", "ruby-exiv2"],
        "Exif.Photo.PixelYDimension" => "32",
        "Exif.Image.ExifTag"         => "52"
      })
    end
    
    it "should set Exif data" do
      @exif_data["Exif.Image.Software"] = "ruby-exiv2"
      expect(@exif_data.to_hash["Exif.Image.Software"]).to eq("ruby-exiv2")
    end
    
    it "should set multiply Exif data values" do
      @exif_data["Exif.Image.Software"] = ["ruby-exiv2", "plasq skitch"]
      expect(@exif_data.to_hash["Exif.Image.Software"]).to eq(["ruby-exiv2", "plasq skitch"])
    end
    
    it "should delete one value of Exif data" do
      @exif_data["Exif.Image.Software"] = ["ruby-exiv2", "plasq skitch"]
      @exif_data.delete("Exif.Image.Software")
      expect(@exif_data.to_hash["Exif.Image.Software"]).to eq("plasq skitch")
    end
    
    it "should delete all values of Exif data" do
      @exif_data.delete_all("Exif.Image.Software")
      expect(@exif_data.to_hash["Exif.Image.Software"]).to eq(nil)
    end
  end

  it "adds #original_value to multi-lang fields" do
    image = Exiv2::ImageFactory.open(Pathname.new("spec/files/photo_with_utf8_description.jpg").to_s)
    image.read_metadata
    # Don't change original string
    image.xmp_data["Xmp.dc.title"].should == "lang=\"x-default\" Pregnant woman shown in nudity."
    # New instance method extracting all alt-values
    image.xmp_data["Xmp.dc.title"].original_value.should == {"x-default"=>"Pregnant woman shown in nudity."}
  end
end
