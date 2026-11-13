#include "pch.h"
#include "CppUnitTest.h"
#define NOMINMAX
#include <Windows.h>
#include <DbgHelp.h>
#include <algorithm>
#include <array>
#include <atomic>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <limits>
#include <stdexcept>
#include <string>
#include <string_view>
#include <vector>

using namespace Microsoft::VisualStudio::CppUnitTestFramework;
using namespace std;

namespace fs = filesystem;

namespace JWlink2Test
{
	namespace
	{
		using Bytes = vector<uint8_t>;

		struct TestRelocation
		{
			uint32_t offset;
			uint32_t symbolIndex;
			uint16_t type;
		};

		struct TestSection
		{
			string name;
			Bytes data;
			uint32_t characteristics = 0x60000020;
			vector<TestRelocation> relocations;
		};

		struct TestSymbol
		{
			string name;
			uint32_t value = 0;
			int32_t section = 0;
			uint8_t storageClass = 2;
			vector<array<uint8_t, 18>> auxiliary;
			vector<array<uint8_t, 20>> bigObjAuxiliary;
		};

		void put16(Bytes& bytes, size_t offset, uint16_t value)
		{
			if (bytes.size() < offset + 2) bytes.resize(offset + 2);
			bytes[offset] = static_cast<uint8_t>(value);
			bytes[offset + 1] = static_cast<uint8_t>(value >> 8);
		}

		void put32(Bytes& bytes, size_t offset, uint32_t value)
		{
			if (bytes.size() < offset + 4) bytes.resize(offset + 4);
			for (size_t i = 0; i < 4; ++i) bytes[offset + i] = static_cast<uint8_t>(value >> (i * 8));
		}

		void put64(Bytes& bytes, size_t offset, uint64_t value)
		{
			if (bytes.size() < offset + 8) bytes.resize(offset + 8);
			for (size_t i = 0; i < 8; ++i) bytes[offset + i] = static_cast<uint8_t>(value >> (i * 8));
		}

		uint16_t get16(const Bytes& bytes, size_t offset)
		{
			Assert::IsTrue(offset + 2 <= bytes.size(), L"PE field exceeds the file");
			return static_cast<uint16_t>(bytes[offset] | (static_cast<uint16_t>(bytes[offset + 1]) << 8));
		}

		uint32_t get32(const Bytes& bytes, size_t offset)
		{
			Assert::IsTrue(offset + 4 <= bytes.size(), L"PE field exceeds the file");
			uint32_t value = 0;
			for (size_t i = 0; i < 4; ++i) value |= static_cast<uint32_t>(bytes[offset + i]) << (8 * i);
			return value;
		}

		uint64_t get64(const Bytes& bytes, size_t offset)
		{
			Assert::IsTrue(offset + 8 <= bytes.size(), L"PE field exceeds the file");
			uint64_t value = 0;
			for (size_t i = 0; i < 8; ++i) value |= static_cast<uint64_t>(bytes[offset + i]) << (8 * i);
			return value;
		}

		void appendString(Bytes& bytes, const string& value)
		{
			bytes.insert(bytes.end(), value.begin(), value.end());
			bytes.push_back(0);
		}

		uint32_t addString(Bytes& table, const string& value)
		{
			const auto offset = static_cast<uint32_t>(table.size());
			appendString(table, value);
			return offset;
		}

		Bytes makeObject(vector<TestSection> sections, vector<TestSymbol> symbols, uint16_t machine = 0x8664)
		{
			Bytes bytes(20 + sections.size() * 40, 0);
			put16(bytes, 0, machine);
			put16(bytes, 2, static_cast<uint16_t>(sections.size()));
			put16(bytes, 16, 0);
			Bytes strings(4, 0);
			vector<size_t> sectionNameOffsets(sections.size(), 0);
			for (size_t i = 0; i < sections.size(); ++i)
			{
				if (sections[i].name.size() > 8) sectionNameOffsets[i] = addString(strings, sections[i].name);
			}
			vector<uint32_t> rawOffsets(sections.size(), 0);
			for (size_t i = 0; i < sections.size(); ++i)
			{
				if (!sections[i].data.empty())
				{
					rawOffsets[i] = static_cast<uint32_t>(bytes.size());
					bytes.insert(bytes.end(), sections[i].data.begin(), sections[i].data.end());
				}
			}
			vector<uint32_t> relocationOffsets(sections.size(), 0);
			for (size_t i = 0; i < sections.size(); ++i)
			{
				if (!sections[i].relocations.empty())
				{
					relocationOffsets[i] = static_cast<uint32_t>(bytes.size());
					if (sections[i].relocations.size() >= 0xffff)
					{
						const auto entry = bytes.size();
						bytes.resize(entry + 10, 0);
						put32(bytes, entry, static_cast<uint32_t>(sections[i].relocations.size() + 1));
					}
					for (const auto& relocation : sections[i].relocations)
					{
						const auto entry = bytes.size();
						bytes.resize(entry + 10, 0);
						put32(bytes, entry, relocation.offset);
						put32(bytes, entry + 4, relocation.symbolIndex);
						put16(bytes, entry + 8, relocation.type);
					}
				}
			}
			const auto symbolOffset = static_cast<uint32_t>(bytes.size());
			uint32_t symbolCount = 0;
			for (const auto& symbol : symbols) symbolCount += static_cast<uint32_t>(1 + symbol.auxiliary.size());
			put32(bytes, 8, symbolOffset);
			put32(bytes, 12, symbolCount);
			for (const auto& symbol : symbols)
			{
				const auto entry = bytes.size();
				bytes.resize(entry + 18, 0);
				if (symbol.name.size() <= 8)
				{
					copy(symbol.name.begin(), symbol.name.end(), bytes.begin() + entry);
				}
				else
				{
					put32(bytes, entry, 0);
					put32(bytes, entry + 4, addString(strings, symbol.name));
				}
				put32(bytes, entry + 8, symbol.value);
				put16(bytes, entry + 12, static_cast<uint16_t>(symbol.section));
				bytes[entry + 16] = symbol.storageClass;
				bytes[entry + 17] = static_cast<uint8_t>(symbol.auxiliary.size());
				for (const auto& auxiliary : symbol.auxiliary) bytes.insert(bytes.end(), auxiliary.begin(), auxiliary.end());
			}
			put32(bytes, bytes.size(), static_cast<uint32_t>(strings.size()));
			bytes.insert(bytes.end(), strings.begin() + 4, strings.end());
			for (size_t i = 0; i < sections.size(); ++i)
			{
				const auto header = 20 + i * 40;
				if (sectionNameOffsets[i] == 0)
				{
					copy(sections[i].name.begin(), sections[i].name.end(), bytes.begin() + header);
				}
				else
				{
					const auto longName = "/" + to_string(sectionNameOffsets[i]);
					copy(longName.begin(), longName.end(), bytes.begin() + header);
				}
				put32(bytes, header + 8, static_cast<uint32_t>(sections[i].data.size()));
				put32(bytes, header + 16, static_cast<uint32_t>(sections[i].data.size()));
				put32(bytes, header + 20, rawOffsets[i]);
				put32(bytes, header + 24, relocationOffsets[i]);
				const bool overflow = sections[i].relocations.size() >= 0xffff;
				put16(bytes, header + 32, overflow ? 0xffff : static_cast<uint16_t>(sections[i].relocations.size()));
				put32(bytes, header + 36, sections[i].characteristics | (overflow ? 0x01000000u : 0u));
			}
			return bytes;
		}

		array<uint8_t, 18> sectionAux(uint32_t dataLength, uint16_t associated, uint8_t selection)
		{
			array<uint8_t, 18> auxiliary{};
			for (size_t i = 0; i < 4; ++i) auxiliary[i] = static_cast<uint8_t>(dataLength >> (i * 8));
			auxiliary[12] = static_cast<uint8_t>(associated);
			auxiliary[13] = static_cast<uint8_t>(associated >> 8);
			auxiliary[14] = selection;
			return auxiliary;
		}

		array<uint8_t, 20> bigObjSectionAux(uint32_t associated, uint8_t selection)
		{
			array<uint8_t, 20> auxiliary{};
			auxiliary[12] = static_cast<uint8_t>(associated);
			auxiliary[13] = static_cast<uint8_t>(associated >> 8);
			auxiliary[18] = static_cast<uint8_t>(associated >> 16);
			auxiliary[19] = static_cast<uint8_t>(associated >> 24);
			auxiliary[16] = selection;
			return auxiliary;
		}

		array<uint8_t, 18> weakAux(uint32_t tagIndex, uint32_t characteristics)
		{
			array<uint8_t, 18> auxiliary{};
			for (size_t i = 0; i < 4; ++i) auxiliary[i] = static_cast<uint8_t>(tagIndex >> (i * 8));
			for (size_t i = 0; i < 4; ++i) auxiliary[4 + i] = static_cast<uint8_t>(characteristics >> (i * 8));
			return auxiliary;
		}

		Bytes makeEntryObject(const string& entry = "main")
		{
			return makeObject({ TestSection{ ".text",{ 0xc3 } } }, { TestSymbol{ entry, 0, 1, 2,{} } });
		}

		TestSection makeDirectiveSection(const string& directives)
		{
			return TestSection{ ".drectve", Bytes(directives.begin(), directives.end()), 0x00100a00 };
		}

		Bytes makeBigObjObject(const vector<TestSection>& sections, const vector<TestSymbol>& symbols)
		{
			Bytes bytes(56 + sections.size() * 40, 0);
			put16(bytes, 0, 0);
			put16(bytes, 2, 0xffff);
			put16(bytes, 4, 2);
			put16(bytes, 6, 0x8664);
			const array<uint8_t, 16> classId{ 0xc7, 0xa1, 0xba, 0xd1, 0xee, 0xba, 0xa9, 0x4b, 0xaf, 0x20, 0xfa, 0xf6, 0x6a, 0xa4, 0xdc, 0xb8 };
			copy(classId.begin(), classId.end(), bytes.begin() + 12);
			put32(bytes, 44, static_cast<uint32_t>(sections.size()));

			Bytes strings(4, 0);
			vector<size_t> sectionNameOffsets(sections.size(), 0);
			for (size_t i = 0; i < sections.size(); ++i)
			{
				if (sections[i].name.size() > 8) sectionNameOffsets[i] = addString(strings, sections[i].name);
			}
			vector<uint32_t> rawOffsets(sections.size(), 0);
			vector<uint32_t> relocationOffsets(sections.size(), 0);
			for (size_t i = 0; i < sections.size(); ++i)
			{
				if (!sections[i].data.empty())
				{
					rawOffsets[i] = static_cast<uint32_t>(bytes.size());
					bytes.insert(bytes.end(), sections[i].data.begin(), sections[i].data.end());
				}
			}
			for (size_t i = 0; i < sections.size(); ++i)
			{
				if (!sections[i].relocations.empty())
				{
					relocationOffsets[i] = static_cast<uint32_t>(bytes.size());
					if (sections[i].relocations.size() >= 0xffff)
					{
						const auto entry = bytes.size();
						bytes.resize(entry + 10, 0);
						put32(bytes, entry, static_cast<uint32_t>(sections[i].relocations.size() + 1));
					}
					for (const auto& relocation : sections[i].relocations)
					{
						const auto entry = bytes.size();
						bytes.resize(entry + 10, 0);
						put32(bytes, entry, relocation.offset);
						put32(bytes, entry + 4, relocation.symbolIndex);
						put16(bytes, entry + 8, relocation.type);
					}
				}
			}
			const auto symbolOffset = static_cast<uint32_t>(bytes.size());
			uint32_t symbolCount = 0;
			for (const auto& symbol : symbols) symbolCount += static_cast<uint32_t>(1 + symbol.bigObjAuxiliary.size());
			put32(bytes, 48, symbolOffset);
			put32(bytes, 52, symbolCount);
			for (const auto& symbol : symbols)
			{
				const auto entry = bytes.size();
				bytes.resize(entry + 20, 0);
				if (symbol.name.size() <= 8)
				{
					copy(symbol.name.begin(), symbol.name.end(), bytes.begin() + entry);
				}
				else
				{
					put32(bytes, entry, 0);
					put32(bytes, entry + 4, addString(strings, symbol.name));
				}
				put32(bytes, entry + 8, symbol.value);
				put32(bytes, entry + 12, static_cast<uint32_t>(symbol.section));
				put16(bytes, entry + 16, 0);
				bytes[entry + 18] = symbol.storageClass;
				bytes[entry + 19] = static_cast<uint8_t>(symbol.bigObjAuxiliary.size());
				for (const auto& auxiliary : symbol.bigObjAuxiliary) bytes.insert(bytes.end(), auxiliary.begin(), auxiliary.end());
			}
			put32(bytes, bytes.size(), static_cast<uint32_t>(strings.size()));
			bytes.insert(bytes.end(), strings.begin() + 4, strings.end());
			for (size_t i = 0; i < sections.size(); ++i)
			{
				const auto header = 56 + i * 40;
				if (sectionNameOffsets[i] == 0)
				{
					copy(sections[i].name.begin(), sections[i].name.end(), bytes.begin() + header);
				}
				else
				{
					const auto longName = "/" + to_string(sectionNameOffsets[i]);
					copy(longName.begin(), longName.end(), bytes.begin() + header);
				}
				put32(bytes, header + 8, static_cast<uint32_t>(sections[i].data.size()));
				put32(bytes, header + 16, static_cast<uint32_t>(sections[i].data.size()));
				put32(bytes, header + 20, rawOffsets[i]);
				put32(bytes, header + 24, relocationOffsets[i]);
				const bool overflow = sections[i].relocations.size() >= 0xffff;
				put16(bytes, header + 32, overflow ? 0xffff : static_cast<uint16_t>(sections[i].relocations.size()));
				put32(bytes, header + 36, sections[i].characteristics | (overflow ? 0x01000000u : 0u));
			}
			return bytes;
		}

		Bytes makeBigObjComdatObject(const string& key, const Bytes& content)
		{
			TestSymbol sectionSymbol{ ".text$comdat", 0, 1, 3,{} };
			sectionSymbol.bigObjAuxiliary.push_back(bigObjSectionAux(0, 2));
			return makeBigObjObject({ TestSection{ ".text$comdat", content, 0x60001020 } }, { sectionSymbol, TestSymbol{ key, 0, 1, 2,{} } });
		}

		Bytes makeComdatObject(const string& key, const Bytes& content, uint8_t selection)
		{
			TestSection section{ ".text$comdat", content, 0x60001020 };
			TestSymbol sectionSymbol{ ".text$comdat", 0, 1, 3,{ sectionAux(static_cast<uint32_t>(content.size()), 0, selection) } };
			TestSymbol keySymbol{ key, 0, 1, 2,{} };
			return makeObject({ section }, { sectionSymbol, keySymbol });
		}

		Bytes makeAssociativeComdatObject(const Bytes& parentData, const Bytes& childData)
		{
			TestSection parent{ ".text$parent", parentData, 0x60001020 };
			TestSection child{ ".xdata$child", childData, 0x40001040 };
			TestSymbol parentSection{ ".text$parent", 0, 1, 3,{ sectionAux(static_cast<uint32_t>(parentData.size()), 0, 2) } };
			TestSymbol parentKey{ "parentKey", 0, 1, 2,{} };
			TestSymbol childSection{ ".xdata$child", 0, 2, 3,{ sectionAux(static_cast<uint32_t>(childData.size()), 1, 5) } };
			return makeObject({ parent, child }, { parentSection, parentKey, childSection });
		}

		Bytes makeImportObject(const string& publicName, const string& dllName, const string& importName, uint16_t hint, uint16_t typeInfo)
		{
			Bytes strings;
			appendString(strings, publicName);
			appendString(strings, dllName);
			if (!importName.empty()) appendString(strings, importName);
			Bytes bytes(20, 0);
			put16(bytes, 0, 0);
			put16(bytes, 2, 0xffff);
			put16(bytes, 6, 0x8664);
			put32(bytes, 12, static_cast<uint32_t>(strings.size()));
			put16(bytes, 16, hint);
			put16(bytes, 18, typeInfo);
			bytes.insert(bytes.end(), strings.begin(), strings.end());
			return bytes;
		}

		void appendArchiveMember(Bytes& archive, const string& name, const Bytes& content)
		{
			auto appendField = [&](const string& value, size_t width)
				{
					string field = value;
					field.resize(width, ' ');
					archive.insert(archive.end(), field.begin(), field.end());
				};
			appendField(name + (name.find('/') == string::npos ? "/" : ""), 16);
			appendField("0", 12);
			appendField("0", 6);
			appendField("0", 6);
			appendField("100644", 8);
			appendField(to_string(content.size()), 10);
			archive.push_back('`');
			archive.push_back('\n');
			archive.insert(archive.end(), content.begin(), content.end());
			if ((archive.size() & 1) != 0) archive.push_back('\n');
		}

		Bytes makeArchive(const vector<pair<string, Bytes>>& members)
		{
			const string magic = "!<arch>\n";
			Bytes bytes(magic.begin(), magic.end());
			for (const auto& member : members) appendArchiveMember(bytes, member.first, member.second);
			return bytes;
		}

		Bytes decodeBase64(string_view encoded)
		{
			constexpr string_view alphabet = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
			Bytes bytes;
			uint32_t accumulator = 0;
			unsigned int bitCount = 0;
			for (const auto ch : encoded)
			{
				if (ch == '=') break;
				const auto value = alphabet.find(ch);
				if (value == string_view::npos) throw runtime_error("invalid embedded MSVC archive fixture");
				accumulator = (accumulator << 6) | static_cast<uint32_t>(value);
				bitCount += 6;
				if (bitCount >= 8)
				{
					bitCount -= 8;
					bytes.push_back(static_cast<uint8_t>(accumulator >> bitCount));
					accumulator &= (uint32_t{ 1 } << bitCount) - 1;
				}
			}
			return bytes;
		}

		Bytes makeMsvcIndexedArchive()
		{
			constexpr string_view encoded =
				"ITxhcmNoPgovICAgICAgICAgICAgICAgMTc5NDI0MzU0MCAgICAgICAgICAgICAgMCAgICAgICAyMiAgICAgICAgYAoAAAABAAAAsmFyY2hpdmVIZWxwZXIALyAgICAgICAgICAgICAgIDE3OTQyNDM1NDAgICAgICAgICAgICAgIDAgICAgICAgMjggICAgICAgIGAKAQAAALIAAAABAAAAAQBhcmNoaXZlSGVscGVyAGhlbHBlci5vYmovICAgICAxNzk0MjQzNTQwICAgICAgICAgICAgICAxMDA2NjYgIDQ5OCAgICAgICBgCmSGAwDU+/FqPgEAAAkAAAAAAAAALnRleHQkbW4AAAAAAAAAAAEAAACMAAAAAAAAAAAAAAAAAAAAIABQYC5kYXRhAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAEAAUMAuZGVidWckUwAAAAAAAAAAsAAAAI0AAAAAAAAAAAAAAAAAAABAABBCwwQAAADxAAAAogAAAGcAAREAAAAAQzpcZml4dHVyZVxoZWxwZXIub2JqAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAAADcAPBEDAgAA0AAAAAAAAAAAAA4ANQA3kAAATWljcm9zb2Z0IChSKSBNYWNybyBBc3NlbWJsZXIAAAAAAEBjb21wLmlkN5ADAf//AAADAEBmZWF0LjAwEAAAAP//AAADAC50ZXh0JG1uAAAAAAEAAAADAQEAAAAAAAAAAAAAAAAAAAAAAC5kYXRhAAAAAAAAAAIAAAADAQAAAAAAAAAAAAAAAAAAAAAAAC5kZWJ1ZyRTAAAAAAMAAAADAbAAAAAAAAAAAAAAAAAAAAAAAAAAAAAEAAAAAAAAAAEAIAACABIAAABhcmNoaXZlSGVscGVyAA==";
			return decodeBase64(encoded);
		}

		Bytes makeMsvcImportArchive()
		{
			constexpr string_view encoded = "ITxhcmNoPgovICAgICAgICAgICAgICAgMTc5NDI0MzY3NiAgICAgICAgICAgICAgMCAgICAgICAxMzYgICAgICAgYAoAAAAFAAABmgAAA8QAAAT6AAAGVAAABlRfX0lNUE9SVF9ERVNDUklQVE9SX2ZpeHR1cmUAX19OVUxMX0lNUE9SVF9ERVNDUklQVE9SAH9maXh0dXJlX05VTExfVEhVTktfREFUQQBfX2ltcF9hcmNoaXZlSGVscGVyAGFyY2hpdmVIZWxwZXIALyAgICAgICAgICAgICAgIDE3OTQyNDM2NzYgICAgICAgICAgICAgIDAgICAgICAgMTQ2ICAgICAgIGAKAQAAALIAAADEAAVQAAFQGAAAFAAAAAQACAAQABAADAF9fSU1QT1JUX0RFU0NSSVBUT1JfZml4dHVyZQBfX05VTExfSU1QT1JUX0RFU0NSSVBUT1IAX19pbXBfYXJjaGl2ZUhlbHBlcgBhcmNoaXZlSGVscGVyAH9maXh0dXJlX05VTExfVEhVTktfREFUQQBmaXh0dXJlLmRsbC8gICAgMTc5NDI0MzY3NiAgICAgICAgICAgICAgMCAgICAgICA0OTMgICAgICAgYApkhgMAXPzxagsBAAAIAAAAAAAAAC5kZWJ1ZyRTAAAAAAAAAABBAAAAjAAAAAAAAAAAAAAAAAAAAEAAEEIuaWRhdGEkMgAAAAAAAAAAFAAAAM0AAADhAAAAAAAAAAMAAABAADDALmlkYXRhJDYAAAAAAAAAAAwAAAD/AAAA4QAAAAAAAAAAAAAAQAAgwAIAAAASAAkAAAAAAAtmaXh0dXJlLmRsbCcAExAHAAAA0AAAAAAAAAAOADUAN5ASTWljcm9zb2Z0IChSKSBMSU5LAAAAAAAAAAAAAAAAAAAAAAAAAAAMAAAAAwAAAAMAAAAAAAQAAAADABAAAAAFAAAAAwBmaXh0dXJlLmRsbABAY29tcC5pZDeQAQH//wAAAwAAAAAABAAAAAAAAAACAAAAAgAuaWRhdGEkMkAAAMACAAAAaAAuaWRhdGEkNgAAAAADAAAAAwAuaWRhdGEkNEAAAMAAAAAAaAAuaWRhdGEkNUAAAMAAAAAAaAAAAAAAIAAAAAAAAAAAAAAAAgAAAAAAOQAAAAAAAAAAAAAAAgBSAAAAX19JTVBPUlRfREVTQ1JJUFRPUl9maXh0dXJlAF9fTlVMTF9JTVBPUlRfREVTQ1JJUFRPUgB/Zml4dHVyZV9OVUxMX1RIVU5LX0RBVEEACmZpeHR1cmUuZGxsLyAgICAxNzk0MjQzNjc2ICAgICAgICAgICAgICAwICAgICAgIDI1MCAgICAgICBgCmSGAgBc/PFquQAAAAIAAAAAAAAALmRlYnVnJFMAAAAAAAAAAEEAAABkAAAAAAAAAAAAAAAAAAAAQAAQQi5pZGF0YSQzAAAAAAAAAAAUAAAApQAAAAAAAAAAAAAAAAAAAEAAMMACAAAAEgAJAAAAAAALZml4dHVyZS5kbGwnABMQBwAAANAAAAAAAAAADgA1ADeQEk1pY3Jvc29mdCAoUikgTElOSwAAAAAAAAAAAAAAAAAAAAAAQGNvbXAuaWQ3kAEB//8AAAMAAAAAAAQAAAAAAAAAAgAAAAIAHQAAAF9fTlVMTF9JTVBPUlRfREVTQ1JJUFRPUgBmaXh0dXJlLmRsbC8gICAgMTc5NDI0MzY3NiAgICAgICAgICAgICAgMCAgICAgICAyODYgICAgICAgYApkhgMAXPzxat0AAAACAAAAAAAAAC5kZWJ1ZyRTAAAAAAAAAABBAAAAjAAAAAAAAAAAAAAAAAAAAEAAEEIuaWRhdGEkNQAAAAAAAAAACAAAAM0AAAAAAAAAAAAAAAAAAABAAEDALmlkYXRhJDQAAAAAAAAAAAgAAADVAAAAAAAAAAAAAAAAAAAAQABAwAIAAAASAAkAAAAAAAtmaXh0dXJlLmRsbCcAExAHAAAA0AAAAAAAAAAOADUAN5ASTWljcm9zb2Z0IChSKSBMSU5LAAAAAAAAAAAAAAAAAAAAAEBjb21wLmlkN5ABAf//AAADAAAAAAAEAAAAAAAAAAIAAAACAB0AAAB/Zml4dXJlX05VTExfVEhVTktfREFUQQBmaXh0dXJlLmRsbwvICAgMTc5NDI0MzY3NiAgICAgICAgICAgICAgMCAgICAgICA0NiAgICAgICAgYGgAAP//AABkhg==";
			return decodeBase64(encoded);
		}

		class Workspace
		{
		public:
			Workspace()
			{
				wchar_t temp[MAX_PATH]{};
				const auto tempLength = GetTempPathW(MAX_PATH, temp);
				Assert::IsTrue(tempLength != 0 && tempLength < MAX_PATH, L"could not locate the temporary directory");
				static atomic<unsigned long long> sequence{ 0 };
				const auto suffix = L"JWlink2Test-" + to_wstring(GetCurrentProcessId()) + L"-" + to_wstring(GetTickCount64()) + L"-" + to_wstring(sequence.fetch_add(1));
				path = fs::path(temp) / suffix;
				fs::create_directories(path);
			}
			~Workspace()
			{
				error_code ignored;
				fs::remove_all(path, ignored);
			}
			fs::path path;
			fs::path file(const wstring& name) const { return path / name; }
			void write(const wstring& name, const Bytes& bytes) const
			{
				ofstream output(file(name), ios::binary);
				Assert::IsTrue(static_cast<bool>(output), L"could not create fixture file");
				output.write(reinterpret_cast<const char*>(bytes.data()), static_cast<streamsize>(bytes.size()));
				Assert::IsTrue(static_cast<bool>(output), L"could not write fixture file");
			}
			void writeText(const wstring& name, const wstring& text) const
			{
				wofstream output(file(name));
				Assert::IsTrue(static_cast<bool>(output), L"could not create text fixture");
				output << text;
				Assert::IsTrue(static_cast<bool>(output), L"could not write text fixture");
			}
			Bytes read(const wstring& name) const
			{
				ifstream input(file(name), ios::binary);
				Assert::IsTrue(static_cast<bool>(input), L"expected output file was not created");
				return Bytes(istreambuf_iterator<char>(input), istreambuf_iterator<char>());
			}
		};

		void ModuleAnchor() {}

		fs::path toolPath(const wchar_t* executableName)
		{
			HMODULE module = nullptr;
			const auto address = reinterpret_cast<LPCWSTR>(&ModuleAnchor);
			Assert::IsTrue(GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS | GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT, address, &module) != 0,
				L"could not locate the JWlink2Test module");
			wchar_t path[32768]{};
			const auto length = GetModuleFileNameW(module, path, static_cast<DWORD>(size(path)));
			Assert::IsTrue(length != 0 && length < size(path), L"could not read the JWlink2Test module path");
			auto directory = fs::path(path).parent_path();
			while (!directory.empty())
			{
				for (const auto& configuration : { L"Debug", L"Release" })
				{
					const auto candidate = directory / L"bin" / L"tools" / configuration / L"x64" / executableName;
					if (fs::exists(candidate)) return candidate;
				}
				directory = directory.parent_path();
			}
			Assert::Fail(L"could not find the required x64 linker build output from the test module location");
			return {};
		}

		fs::path jwlink2Path() { return toolPath(L"JWlink2.exe"); }
		fs::path jwlinkPath() { return toolPath(L"JWlink.exe"); }

		wstring quoteArgument(const wstring& value)
		{
			wstring quoted = L"\"";
			size_t slashes = 0;
			for (const auto ch : value)
			{
				if (ch == L'\\')
				{
					++slashes;
				}
				else if (ch == L'\"')
				{
					quoted.append(slashes * 2 + 1, L'\\');
					quoted.push_back(ch);
					slashes = 0;
				}
				else
				{
					quoted.append(slashes, L'\\');
					quoted.push_back(ch);
					slashes = 0;
				}
			}
			quoted.append(slashes * 2, L'\\');
			quoted.push_back(L'\"');
			return quoted;
		}

		struct ProcessResult
		{
			DWORD exitCode = 0;
			string output;
		};

		ProcessResult runExecutable(const Workspace& workspace, const fs::path& executable, const vector<wstring>& arguments, const vector<pair<wstring, wstring>>& environment = {}, bool quoteAll = true)
		{
			SECURITY_ATTRIBUTES security{ sizeof(SECURITY_ATTRIBUTES), nullptr, TRUE };
			HANDLE readPipe = nullptr;
			HANDLE writePipe = nullptr;
			Assert::IsTrue(CreatePipe(&readPipe, &writePipe, &security, 0) != 0, L"could not create process output pipe");
			SetHandleInformation(readPipe, HANDLE_FLAG_INHERIT, 0);
			STARTUPINFOW startup{};
			startup.cb = sizeof(startup);
			startup.dwFlags = STARTF_USESTDHANDLES;
			startup.hStdOutput = writePipe;
			startup.hStdError = writePipe;
			startup.hStdInput = GetStdHandle(STD_INPUT_HANDLE);
			PROCESS_INFORMATION process{};
			wstring command = quoteArgument(executable.wstring());
			for (const auto& argument : arguments)
			{
				command.push_back(L' ');
				command += quoteAll || argument.empty() || argument.find_first_of(L" \t\"") != wstring::npos ? quoteArgument(argument) : argument;
			}
			vector<wchar_t> mutableCommand(command.begin(), command.end());
			mutableCommand.push_back(L'\0');
			vector<wchar_t> block;
			if (!environment.empty())
			{
				auto inherited = GetEnvironmentStringsW();
				Assert::IsNotNull(inherited, L"could not read process environment");
				for (const wchar_t* item = inherited; *item; item += wcslen(item) + 1)
				{
					const wstring entry(item);
					const auto equals = entry.find(L'=', entry[0] == L'=' ? 1 : 0);
					const auto overridden = any_of(environment.begin(), environment.end(), [&](const auto& pair) { return equals != wstring::npos && _wcsicmp(entry.substr(0, equals).c_str(), pair.first.c_str()) == 0; });
					if (!overridden) { block.insert(block.end(), entry.begin(), entry.end()); block.push_back(L'\0'); }
				}
				FreeEnvironmentStringsW(inherited);
				for (const auto& [name, value] : environment)
				{
					if (value.empty()) continue;
					const auto entry = name + L"=" + value;
					block.insert(block.end(), entry.begin(), entry.end());
					block.push_back(L'\0');
				}
				block.push_back(L'\0');
			}
			const auto created = CreateProcessW(nullptr, mutableCommand.data(), nullptr, nullptr, TRUE, CREATE_NO_WINDOW | (environment.empty() ? 0 : CREATE_UNICODE_ENVIRONMENT), environment.empty() ? nullptr : block.data(), workspace.path.c_str(), &startup, &process);
			CloseHandle(writePipe);
			Assert::IsTrue(created != 0, L"could not start linker process");
			ProcessResult result;
			char buffer[2048];
			DWORD count = 0;
			while (ReadFile(readPipe, buffer, sizeof(buffer), &count, nullptr) && count != 0) result.output.append(buffer, count);
			CloseHandle(readPipe);
			const auto wait = WaitForSingleObject(process.hProcess, 10000);
			if (wait == WAIT_TIMEOUT)
			{
				TerminateProcess(process.hProcess, 0xffff);
				WaitForSingleObject(process.hProcess, INFINITE);
			}
			Assert::IsTrue(wait == WAIT_OBJECT_0, L"linker did not exit within 10 seconds");
			GetExitCodeProcess(process.hProcess, &result.exitCode);
			CloseHandle(process.hThread);
			CloseHandle(process.hProcess);
			return result;
		}

		ProcessResult runLinker(const Workspace& workspace, const vector<wstring>& arguments)
		{
			return runExecutable(workspace, jwlink2Path(), arguments);
		}

		ProcessResult runJwlink(const Workspace& workspace, const vector<wstring>& inputs, const wstring& outputName)
		{
			vector<wstring> arguments{ L"format", L"win", L"pe", L"option", L"quiet,start=main", L"file" };
			wstring inputList;
			for (const auto& input : inputs)
			{
				if (!inputList.empty()) inputList += L',';
				inputList += input;
			}
			arguments.push_back(move(inputList));
			arguments.push_back(L"name");
			arguments.push_back(workspace.file(outputName).wstring());
			return runExecutable(workspace, jwlinkPath(), arguments);
		}

		vector<wstring> outputArgs(const Workspace& workspace, const wstring& outputName = L"linked.exe")
		{
			return { L"/OUT:" + workspace.file(outputName).wstring(), L"/ENTRY:main" };
		}

		ProcessResult linkObject(const Workspace& workspace, const Bytes& object, vector<wstring> args = {})
		{
			workspace.write(L"input.obj", object);
			if (args.empty()) args = outputArgs(workspace);
			args.push_back(workspace.file(L"input.obj").wstring());
			return runLinker(workspace, args);
		}

		void assertSuccess(const ProcessResult& result)
		{
			const wstring diagnostic(result.output.begin(), result.output.end());
			Assert::IsTrue(result.exitCode == 0, diagnostic.c_str());
		}

		void assertFailureContains(const ProcessResult& result, const string& expected)
		{
			Assert::IsTrue(result.exitCode != 0, L"link should fail");
			const wstring diagnostic(result.output.begin(), result.output.end());
			Assert::IsTrue(result.output.find(expected) != string::npos, diagnostic.c_str());
		}

		size_t peHeader(const Bytes& image)
		{
			Assert::IsTrue(image.size() >= 0x40, L"PE image is shorter than its DOS header");
			Assert::AreEqual<uint16_t>(0x5a4d, get16(image, 0), L"DOS signature");
			const auto offset = static_cast<size_t>(get32(image, 0x3c));
			Assert::IsTrue(offset <= image.size() && image.size() - offset >= 24, L"PE header is outside the file");
			Assert::AreEqual<uint32_t>(0x00004550, get32(image, offset), L"PE signature");
			return offset;
		}

		size_t optionalHeader(const Bytes& image)
		{
			const auto pe = peHeader(image);
			const auto coff = pe + 4;
			const auto optional = coff + 20;
			const auto optionalSize = get16(image, coff + 16);
			Assert::IsTrue(optional <= image.size() && optionalSize <= image.size() - optional, L"optional header is outside the file");
			Assert::IsTrue(optionalSize >= 2, L"optional header is missing its format field");
			Assert::AreEqual<uint16_t>(0x20b, get16(image, optional), L"PE32+ optional header");
			return optional;
		}

		size_t sectionTable(const Bytes& image)
		{
			const auto pe = peHeader(image);
			const auto coff = pe + 4;
			return optionalHeader(image) + get16(image, coff + 16);
		}

		uint16_t peFileCharacteristics(const Bytes& image)
		{
			return get16(image, peHeader(image) + 4 + 18);
		}

		bool hasSection(const Bytes& image, const string& name)
		{
			const auto pe = peHeader(image);
			const auto sectionCount = get16(image, pe + 4 + 2);
			const auto table = sectionTable(image);
			for (size_t i = 0; i < sectionCount; ++i)
			{
				const auto header = table + i * 40;
				const string sectionName(reinterpret_cast<const char*>(image.data() + header), strnlen_s(reinterpret_cast<const char*>(image.data() + header), 8));
				if (sectionName == name) return true;
			}
			return false;
		}

		size_t sectionHeader(const Bytes& image, const string& name)
		{
			const auto pe = peHeader(image);
			const auto sectionCount = get16(image, pe + 4 + 2);
			const auto table = sectionTable(image);
			for (size_t i = 0; i < sectionCount; ++i)
			{
				const auto header = table + i * 40;
				string sectionName(reinterpret_cast<const char*>(image.data() + header), strnlen_s(reinterpret_cast<const char*>(image.data() + header), 8));
				if (sectionName == name) return header;
			}
			Assert::Fail(L"requested PE section is missing");
			return 0;
		}

		size_t rvaOffset(const Bytes& image, uint32_t rva);

		// Checks the invariants the Windows loader and PE inspection tools (dumpbin, pefile) enforce on an image layout.
		void validatePeLayout(const Bytes& image)
		{
			const auto pe = peHeader(image);
			const auto optional = optionalHeader(image);
			const auto count = get16(image, pe + 6);
			const auto table = sectionTable(image);
			const auto sectionAlignment = get32(image, optional + 32);
			const auto fileAlignment = get32(image, optional + 36);
			const auto imageSize = get32(image, optional + 56);
			const auto headersSize = get32(image, optional + 60);
			const auto imageBase = get64(image, optional + 24);
			Assert::AreEqual<uint16_t>(0x8664, get16(image, pe + 4), L"machine is AMD64");
			Assert::IsTrue(count > 0 && count <= 96, L"section count is plausible");
			Assert::IsTrue(sectionAlignment >= 0x1000 && (sectionAlignment & (sectionAlignment - 1)) == 0, L"section alignment is a power of two >= 4 KiB");
			Assert::IsTrue(fileAlignment >= 0x200 && fileAlignment <= 0x10000 && (fileAlignment & (fileAlignment - 1)) == 0, L"file alignment is a power of two in [512, 64K]");
			Assert::IsTrue(fileAlignment <= sectionAlignment, L"file alignment does not exceed section alignment");
			Assert::AreEqual<uint32_t>(0, imageBase & 0xffff, L"image base is 64-KiB aligned");
			Assert::AreEqual<uint32_t>(0, headersSize % fileAlignment, L"size of headers is file-aligned");
			Assert::IsTrue(table + static_cast<size_t>(count) * 40 <= headersSize, L"section table fits inside the headers");
			Assert::AreEqual<uint32_t>(0, imageSize % sectionAlignment, L"size of image is section-aligned");

			uint32_t previousEnd = (headersSize + sectionAlignment - 1) / sectionAlignment * sectionAlignment;
			uint32_t previousRawEnd = headersSize;
			uint32_t codeSize = 0, initializedSize = 0, uninitializedSize = 0;
			bool entryInExecutableSection = get32(image, optional + 16) == 0;
			for (size_t i = 0; i < count; ++i)
			{
				const auto header = table + i * 40;
				const auto virtualSize = get32(image, header + 8);
				const auto rva = get32(image, header + 12);
				const auto rawSize = get32(image, header + 16);
				const auto rawPointer = get32(image, header + 20);
				const auto flags = get32(image, header + 36);
				Assert::AreEqual<uint32_t>(0, rva % sectionAlignment, L"section RVA is section-aligned");
				Assert::IsTrue(rva >= previousEnd, L"sections ascend in RVA order without overlap");
				Assert::AreEqual<uint32_t>(0, rawSize % fileAlignment, L"raw size is file-aligned");
				if (rawSize != 0)
				{
					Assert::AreEqual<uint32_t>(0, rawPointer % fileAlignment, L"raw pointer is file-aligned");
					Assert::IsTrue(rawPointer >= previousRawEnd, L"raw data does not overlap the headers or a prior section");
					Assert::IsTrue(static_cast<uint64_t>(rawPointer) + rawSize <= image.size(), L"raw data lies inside the file");
					previousRawEnd = rawPointer + rawSize;
				}
				Assert::IsTrue(virtualSize != 0 || rawSize != 0, L"section is not empty");
				previousEnd = rva + (max(virtualSize, rawSize) + sectionAlignment - 1) / sectionAlignment * sectionAlignment;
				Assert::IsTrue((flags & 0x20) == 0 || (flags & 0x20000000) != 0, L"code sections are executable");
				Assert::IsTrue((flags & 0x20000000) == 0 || (flags & 0x80000000) == 0, L"sections are not both executable and writable");
				if ((flags & 0x20) != 0) codeSize += rawSize;
				else if ((flags & 0x40) != 0) initializedSize += rawSize;
				else if ((flags & 0x80) != 0) uninitializedSize += (max(virtualSize, rawSize) + fileAlignment - 1) / fileAlignment * fileAlignment;
				const auto entry = get32(image, optional + 16);
				if (entry != 0 && entry >= rva && entry < rva + max(virtualSize, rawSize) && (flags & 0x20000000) != 0) entryInExecutableSection = true;
			}
			Assert::AreEqual<uint32_t>(previousEnd, imageSize, L"size of image covers every section");
			Assert::IsTrue(entryInExecutableSection, L"entry point lies in an executable section");
			Assert::AreEqual<uint32_t>(codeSize, get32(image, optional + 4), L"size of code matches code sections");
			Assert::AreEqual<uint32_t>(initializedSize, get32(image, optional + 8), L"size of initialized data matches");
			Assert::AreEqual<uint32_t>(uninitializedSize, get32(image, optional + 12), L"size of uninitialized data matches");

			const auto directoryCount = get32(image, optional + 108);
			Assert::IsTrue(directoryCount <= 16 && get16(image, pe + 20) >= 112 + directoryCount * 8, L"data directory count fits the optional header");
			for (uint32_t i = 0; i < directoryCount; ++i)
			{
				const auto rva = get32(image, optional + 112 + i * 8);
				const auto size = get32(image, optional + 116 + i * 8);
				if (i == 4 || (rva == 0 && size == 0)) continue;
				Assert::IsTrue(rva != 0 && size != 0, L"data directory has both an RVA and a size");
				Assert::IsTrue(static_cast<uint64_t>(rva) + size <= imageSize, L"data directory lies inside the image");
			}

			if ((peFileCharacteristics(image) & 1) == 0)
			{
				const auto relocRva = get32(image, optional + 152);
				const auto relocSize = get32(image, optional + 156);
				const auto relocStart = relocSize == 0 ? 0 : rvaOffset(image, relocRva);
				uint32_t cursor = 0;
				while (cursor < relocSize)
				{
					const auto pageRva = get32(image, relocStart + cursor);
					const auto blockSize = get32(image, relocStart + cursor + 4);
					Assert::IsTrue(blockSize >= 8 && blockSize % 4 == 0 && cursor + blockSize <= relocSize, L"base relocation block is well formed");
					Assert::AreEqual<uint32_t>(0, pageRva % 0x1000, L"base relocation page is 4-KiB aligned");
					Assert::IsTrue(pageRva < imageSize, L"base relocation page lies inside the image");
					for (uint32_t entry = 8; entry < blockSize; entry += 2)
					{
						const auto value = get16(image, relocStart + cursor + entry);
						const auto type = value >> 12;
						Assert::IsTrue(type == 0 || type == 3 || type == 10, L"base relocation type is ABSOLUTE, HIGHLOW, or DIR64");
						Assert::IsTrue(pageRva + (value & 0xfff) < imageSize, L"base relocation target lies inside the image");
					}
					cursor += blockSize;
				}
			}
		}

		size_t rvaOffset(const Bytes& image, uint32_t rva)
		{
			const auto pe = peHeader(image);
			const auto table = sectionTable(image);
			const auto count = get16(image, pe + 4 + 2);
			for (size_t i = 0; i < count; ++i)
			{
				const auto section = table + i * 40;
				const auto virtualSize = get32(image, section + 8);
				const auto sectionRva = get32(image, section + 12);
				const auto rawSize = get32(image, section + 16);
				if (rva >= sectionRva && static_cast<uint64_t>(rva) < static_cast<uint64_t>(sectionRva) + max(virtualSize, rawSize))
				{
					const auto delta = rva - sectionRva;
					Assert::IsTrue(delta < rawSize, L"RVA does not refer to initialized file data");
					return get32(image, section + 20) + delta;
				}
			}
			Assert::Fail(L"RVA does not map to a PE section");
			return 0;
		}

		string cStringAtRva(const Bytes& image, uint32_t rva)
		{
			const auto offset = rvaOffset(image, rva);
			size_t end = offset;
			while (end < image.size() && image[end] != 0) ++end;
			Assert::IsTrue(end < image.size(), L"RVA string is not terminated");
			return string(reinterpret_cast<const char*>(image.data() + offset), end - offset);
		}

		void assertHasBaseReloc(const Bytes& image, uint32_t targetRva, uint16_t expectedType)
		{
			const auto optional = optionalHeader(image);
			const auto relocRva = get32(image, optional + 152);
			const auto relocSize = get32(image, optional + 156);
			Assert::IsTrue(relocRva != 0 && relocSize >= 8, L"base-relocation directory should be present");
			const auto directory = rvaOffset(image, relocRva);
			const auto page = targetRva & ~uint32_t{ 0xfff };
			size_t cursor = directory;
			const auto limit = directory + relocSize;
			while (cursor + 8 <= limit)
			{
				const auto blockPage = get32(image, cursor);
				const auto blockSize = get32(image, cursor + 4);
				Assert::IsTrue(blockSize >= 8 && cursor + blockSize <= limit, L"base-relocation block size must be valid");
				if (blockPage == page)
				{
					for (size_t entry = cursor + 8; entry + 2 <= cursor + blockSize; entry += 2)
					{
						const auto value = get16(image, entry);
						if ((value >> 12) == expectedType && (blockPage + (value & 0x0fff)) == targetRva) return;
					}
				}
				cursor += blockSize;
			}
			Assert::Fail(L"expected base-relocation entry is missing");
		}

		void assertNoOutput(const Workspace& workspace, const wstring& outputName = L"linked.exe")
		{
			Assert::IsFalse(fs::exists(workspace.file(outputName)), L"failed link must not create an output image");
		}
	}

	TEST_CLASS(JWlink2LinkerTests)
	{
	public:
		TEST_METHOD(BigObjSupportsWideSectionNumbersSymbolsAndRelocations)
		{
			Workspace workspace;
			vector<TestSection> sections(32768, TestSection{ ".data",{ 0x5a }, 0xc0000040 });
			TestSection text{ ".text", Bytes(4, 0) };
			text.relocations = { { 0, 1, 4 } };
			sections.push_back(move(text));
			const auto object = makeBigObjObject(move(sections), {
				TestSymbol{ "main", 0, 32769, 2,{} }, TestSymbol{ "targetFunction", 2, 32769, 2,{} }
				});
			assertSuccess(linkObject(workspace, object));
			const auto image = workspace.read(L"linked.exe");
			const auto textHeader = sectionHeader(image, ".text");
			Assert::AreEqual<uint32_t>(get32(image, textHeader + 12), get32(image, optionalHeader(image) + 16), L"32-bit section index resolves the entry point");
			Assert::AreEqual<uint32_t>(static_cast<uint32_t>(-2), get32(image, get32(image, textHeader + 20)), L"20-byte BigObj symbol records resolve AMD64 relocations");
			Assert::AreEqual<uint32_t>(32768, get32(image, sectionHeader(image, ".data") + 8), L"all BigObj sections are parsed and merged");
		}

		TEST_METHOD(BigObjSectionAuxiliaryRecordSelectsComdatWinner)
		{
			Workspace workspace;
			workspace.write(L"entry.obj", makeEntryObject());
			workspace.write(L"first.obj", makeBigObjComdatObject("bigObjComdat", { 0x41 }));
			workspace.write(L"second.obj", makeBigObjComdatObject("bigObjComdat", { 0x42 }));
			auto args = outputArgs(workspace);
			for (const auto& file : { L"entry.obj", L"first.obj", L"second.obj" }) args.push_back(workspace.file(file).wstring());
			assertSuccess(runLinker(workspace, args));
			const auto image = workspace.read(L"linked.exe");
			const auto text = sectionHeader(image, ".text");
			const auto raw = get32(image, text + 20);
			Assert::IsTrue(find(image.begin() + raw, image.begin() + raw + get32(image, text + 16), 0x41) != image.begin() + raw + get32(image, text + 16), L"first BigObj COMDAT candidate is selected");
			Assert::IsTrue(find(image.begin() + raw, image.begin() + raw + get32(image, text + 16), 0x42) == image.begin() + raw + get32(image, text + 16), L"losing BigObj COMDAT candidate is discarded");
		}

		TEST_METHOD(BigObjArchiveMemberCanSatisfyExternalSymbol)
		{
			Workspace workspace;
			TestSection text{ ".text", Bytes(4, 0) };
			text.relocations = { { 0, 1, 4 } };
			workspace.write(L"input.obj", makeObject({ text }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "bigObjHelper", 0, 0, 2,{} } }));
			workspace.write(L"bigobj.lib", makeArchive({ { "bighelper.obj", makeBigObjObject({ TestSection{ ".text",{ 0xc3 } } },{ TestSymbol{ "bigObjHelper", 0, 1, 2,{} } }) } }));
			auto args = outputArgs(workspace);
			args.push_back(workspace.file(L"input.obj").wstring());
			args.push_back(workspace.file(L"bigobj.lib").wstring());
			assertSuccess(runLinker(workspace, args));
			Assert::IsTrue(sectionHeader(workspace.read(L"linked.exe"), ".text") != 0, L"BigObj archive member is selected and linked");
		}

		TEST_METHOD(InvalidBigObjClassIdentifierIsRejected)
		{
			Workspace workspace;
			auto object = makeBigObjObject({ TestSection{ ".text",{ 0xc3 } } }, { TestSymbol{ "main", 0, 1, 2,{} } });
			object[12] ^= 0x01;
			const auto result = linkObject(workspace, object);
			assertFailureContains(result, "unsupported anonymous COFF object signature");
			assertNoOutput(workspace);
		}

		TEST_METHOD(SubsectionsAreOrderedStablyAcrossObjectsAndArchives)
		{
			Workspace workspace;
			const uint32_t flags = 0x40000040;
			TestSection references{ ".data", Bytes(24, 0), 0xc0000040 };
			references.relocations = { { 0, 1, 1 }, { 8, 2, 1 }, { 16, 3, 1 } };
			workspace.write(L"entry.obj", makeObject({ TestSection{ ".text",{ 0xc3 } }, references, TestSection{ ".order$Z",{ 0x7a }, flags }, TestSection{ ".order$M",{ 0x31 }, flags } },
				{ TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "early", 0, 0, 2,{} }, TestSymbol{ "late", 0, 3, 2,{} }, TestSymbol{ "tie2", 0, 0, 2,{} } }));
			workspace.write(L"second.obj", makeObject({ TestSection{ ".order$M",{ 0x32 }, flags }, TestSection{ ".order",{ 0x30 }, flags } }, { TestSymbol{ "tie2", 0, 1, 2,{} } }));
			workspace.write(L"extras.lib", makeArchive({ { "early.obj", makeObject({ TestSection{ ".order$A",{ 0x41 }, flags | 0x00500000 } }, { TestSymbol{ "early", 0, 1, 2,{} } }) } }));
			const vector<wstring> args{ L"/ENTRY:main", L"/OUT:ordered.exe", L"entry.obj", L"second.obj", L"extras.lib" };
			assertSuccess(runLinker(workspace, args));
			const auto image = workspace.read(L"ordered.exe");
			const auto orderHeader = sectionHeader(image, ".order");
			const auto raw = get32(image, orderHeader + 20);
			const auto base = get64(image, optionalHeader(image) + 24) + get32(image, orderHeader + 12);
			Assert::AreEqual<uint8_t>(0x30, image[raw]);
			Assert::AreEqual<uint8_t>(0x41, image[raw + 16], L"archive subsection honors 16-byte alignment");
			Assert::AreEqual<uint8_t>(0x31, image[raw + 17], L"equal names retain object order");
			Assert::AreEqual<uint8_t>(0x32, image[raw + 18]);
			Assert::AreEqual<uint8_t>(0x7a, image[raw + 19]);
			const auto data = get32(image, sectionHeader(image, ".data") + 20);
			Assert::AreEqual<uint64_t>(base + 16, get64(image, data));
			Assert::AreEqual<uint64_t>(base + 19, get64(image, data + 8));
			Assert::AreEqual<uint64_t>(base + 18, get64(image, data + 16));
			assertSuccess(runLinker(workspace, args));
			Assert::IsTrue(image == workspace.read(L"ordered.exe"), L"repeat links preserve deterministic ordering");
		}

		TEST_METHOD(SortedAssociativeComdatsKeepWinnerAndRelocationMappings)
		{
			Workspace workspace;
			auto associative = [](uint8_t marker)
			{
				TestSection parent{ ".text$M", { 0xc3 }, 0x60001020 };
				TestSection child{ ".order$M", { marker }, 0x40001040 };
				TestSection reference{ ".refs", Bytes(8, 0), 0x40000040 };
				reference.relocations = { { 0, 3, 1 } };
				return makeObject({ parent, child, reference }, {
					TestSymbol{ ".text$M", 0, 1, 3,{ sectionAux(1, 0, 2) } }, TestSymbol{ "parentKey", 0, 1, 2,{} },
					TestSymbol{ ".order$M", 0, 2, 3,{ sectionAux(1, 1, 5) } }, TestSymbol{ "childLocal", 0, 2, 3,{} }
					});
			};
			workspace.write(L"entry.obj", makeObject({ TestSection{ ".text",{ 0xc3 } }, TestSection{ ".order$Z",{ 0x7a }, 0x40000040 } }, { TestSymbol{ "main", 0, 1, 2,{} } }));
			workspace.write(L"winner.obj", associative(0x51));
			workspace.write(L"loser.obj", associative(0x52));
			workspace.write(L"early.obj", makeObject({ TestSection{ ".order$A",{ 0x41 }, 0x40000040 } }, {}));
			assertSuccess(runLinker(workspace, { L"/ENTRY:main", L"/OUT:comdat.exe", L"entry.obj", L"winner.obj", L"loser.obj", L"early.obj" }));
			const auto image = workspace.read(L"comdat.exe");
			const auto order = sectionHeader(image, ".order");
			const auto raw = get32(image, order + 20);
			Assert::AreEqual<uint8_t>(0x41, image[raw]);
			Assert::AreEqual<uint8_t>(0x51, image[raw + 1]);
			Assert::AreEqual<uint8_t>(0x7a, image[raw + 2]);
			const auto expected = get64(image, optionalHeader(image) + 24) + get32(image, order + 12) + 1;
			const auto references = get32(image, sectionHeader(image, ".refs") + 20);
			Assert::AreEqual<uint64_t>(expected, get64(image, references));
			Assert::AreEqual<uint64_t>(expected, get64(image, references + 8), L"losing associative section resolves to the selected child");
		}

		TEST_METHOD(JwlinkSubsectionOrderingIsMeasuredSeparately)
		{
			Workspace workspace;
			workspace.write(L"input.obj", makeObject({ TestSection{ ".text", { 0xc3 } }, TestSection{ ".seq$Z", { 0x7a }, 0x40000040 }, TestSection{ ".seq$A", { 0x41 }, 0x40000040 } }, { TestSymbol{ "main", 0, 1, 2,{} } }));
			assertSuccess(runLinker(workspace, { L"/ENTRY:main", L"/OUT:new.exe", L"input.obj" }));
			assertSuccess(runJwlink(workspace, { L"input.obj" }, L"old.exe"));
			const auto modern = workspace.read(L"new.exe");
			const auto original = workspace.read(L"old.exe");
			const auto newRaw = get32(modern, sectionHeader(modern, ".seq") + 20);
			Assert::AreEqual<uint8_t>(0x41, modern[newRaw], L"JWlink2 uses lexical subsection order");
			Assert::AreEqual<uint8_t>(0x7a, modern[newRaw + 1]);
			const auto oldSection = sectionHeader(original, ".rdata");
			const auto oldRaw = get32(original, oldSection + 20);
			const auto oldSize = get32(original, oldSection + 8);
			Assert::IsTrue(oldSize >= 2 && oldSize <= get32(original, oldSection + 16));
			const auto first = find(original.begin() + oldRaw, original.begin() + oldRaw + oldSize, uint8_t{ 0x41 });
			const auto second = find(original.begin() + oldRaw, original.begin() + oldRaw + oldSize, uint8_t{ 0x7a });
			Assert::IsTrue(first != original.begin() + oldRaw + oldSize && second != original.begin() + oldRaw + oldSize && first < second, L"JWlink also orders A before Z, but places them in .rdata with different padding");
		}

		TEST_METHOD(OrderedSubsectionsExecuteTlsCallbacksAndConstructors)
		{
			Workspace workspace;
			TestSection entry{ ".text", { 0x48, 0x83, 0xec, 0x28, 0xff, 0x15, 0, 0, 0, 0, 0xff, 0x15, 8, 0, 0, 0, 0x8b, 0x0d, 0, 0, 0, 0, 0xe8, 0, 0, 0, 0, 0xcc } };
			entry.relocations = { { 6, 1, 4 }, { 12, 1, 4 }, { 18, 2, 4 }, { 23, 3, 4 } };
			TestSection tlsDirectory{ ".rdata$T", Bytes(40, 0), 0x40000040 };
			tlsDirectory.relocations = { { 0, 5, 1 }, { 8, 6, 1 }, { 16, 7, 1 }, { 24, 8, 1 } };
			workspace.write(L"entry.obj", makeObject({ entry, tlsDirectory, TestSection{ ".data", Bytes(12, 0), 0xc0000040 }, TestSection{ ".tls$ZZZ", Bytes(8, 0), 0xc0400040 } },
				{ TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "ctorStart", 0, 0, 2,{} }, TestSymbol{ "state", 0, 3, 2,{} }, TestSymbol{ "ExitProcess", 0, 0, 2,{} }, TestSymbol{ "__tls_used", 0, 2, 2,{} }, TestSymbol{ "tlsStart", 4, 3, 2,{} }, TestSymbol{ "tlsEnd", 5, 3, 2,{} }, TestSymbol{ "tlsIndex", 8, 3, 2,{} }, TestSymbol{ "callbackStart", 0, 0, 2,{} } }));
			auto contribution = [](const string& sectionName, const string& function, const string& pointer, uint8_t digit)
			{
				TestSection code{ ".text", { 0x6b, 0x05, 0, 0, 0, 0, 10, 0x83, 0xc0, digit, 0x89, 0x05, 0, 0, 0, 0, 0xc3 } };
				code.relocations = { { 2, 1, 5 }, { 12, 1, 4 } };
				TestSection slot{ sectionName, Bytes(8, 0), 0xc0400040 };
				slot.relocations = { { 0, 0, 1 } };
				return makeObject({ code, slot }, { TestSymbol{ function, 0, 1, 2,{} }, TestSymbol{ "state", 0, 0, 2,{} }, TestSymbol{ pointer, 0, 2, 2,{} } });
			};
			workspace.write(L"late.obj", contribution(".CRT$XCV", "ctorB", "ctorSlotB", 4));
			workspace.write(L"lateTls.obj", contribution(".tls$BBB", "callbackB", "callbackSlotB", 2));
			workspace.write(L"early.lib", makeArchive({ { "ctor.obj", contribution(".CRT$XCU", "ctorA", "ctorStart", 3) }, { "tls.obj", contribution(".tls$AAA", "callbackA", "callbackStart", 1) } }));
			workspace.write(L"kernel.lib", makeArchive({ { "exit.obj", makeImportObject("ExitProcess", "KERNEL32.dll", "ExitProcess", 0, 4) } }));
			assertSuccess(runLinker(workspace, { L"/ENTRY:main", L"/OUT:ordered.exe", L"entry.obj", L"late.obj", L"lateTls.obj", L"early.lib", L"kernel.lib" }));
			const auto image = workspace.read(L"ordered.exe");
			const auto crt = sectionHeader(image, ".CRT");
			const auto tls = sectionHeader(image, ".tls");
			const auto imageBase = get64(image, optionalHeader(image) + 24);
			for (const auto header : { crt, tls })
			{
				const auto raw = get32(image, header + 20);
				const auto first = get64(image, raw) - imageBase;
				const auto second = get64(image, raw + 8) - imageBase;
				Assert::IsTrue(first != second && first <= UINT32_MAX && second <= UINT32_MAX, L"ordered pointers resolve to distinct functions");
				Assert::AreEqual<uint8_t>(header == crt ? 3 : 1, image[rvaOffset(image, static_cast<uint32_t>(first)) + 9]);
				Assert::AreEqual<uint8_t>(header == crt ? 4 : 2, image[rvaOffset(image, static_cast<uint32_t>(second)) + 9]);
			}
			Assert::AreEqual<uint64_t>(0, get64(image, get32(image, tls + 20) + 16), L"callback array is null-terminated");
			Assert::AreEqual<DWORD>(1234, runExecutable(workspace, workspace.file(L"ordered.exe"), {}).exitCode, L"loader callbacks run before ordered constructors");
		}

		TEST_METHOD(RuntimeMetadataDirectoriesArePreserved)
		{
			Workspace workspace;
			TestSection text{ ".text",{ 0xc3, 0xc3 } };
			TestSection pdata{ ".pdata", Bytes(12, 0), 0x40000040 };
			pdata.relocations = { { 0, 0, 3 },{ 4, 1, 3 },{ 8, 2, 3 } };
			TestSection xdata{ ".xdata",{ 1, 0, 0, 0 }, 0x40000040 };
			TestSection tlsDirectory{ ".rdata$T", Bytes(40, 0), 0x40000040 };
			tlsDirectory.relocations = { { 0, 4, 1 },{ 8, 5, 1 },{ 16, 6, 1 } };
			TestSection tlsTemplate{ ".tls$",{ 0x31, 0x32, 0, 0 }, 0xc0000040 };
			TestSection tlsIndex{ ".data$t", Bytes(4, 0), 0xc0000040 };
			TestSection loadConfig{ ".rdata$L", Bytes(0x70, 0), 0x40000040 };
			put32(loadConfig.data, 0, 0x70);
			TestSection resources{ ".rsrc$01", Bytes(16, 0), 0x40000040 };
			const auto object = makeObject({ text, pdata, xdata, tlsDirectory, tlsTemplate, tlsIndex, loadConfig, resources }, {
				TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "functionEnd", 1, 1, 2,{} }, TestSymbol{ "unwindInfo", 0, 3, 2,{} },
				TestSymbol{ "__tls_used", 0, 4, 2,{} }, TestSymbol{ "tlsStart", 0, 5, 2,{} }, TestSymbol{ "tlsEnd", 4, 5, 2,{} },
				TestSymbol{ "tlsIndex", 0, 6, 2,{} }, TestSymbol{ "__load_config_used", 0, 7, 2,{} }
				});
			assertSuccess(linkObject(workspace, object));
			const auto image = workspace.read(L"linked.exe");
			const auto optional = optionalHeader(image);
			auto verifyDirectory = [&](size_t directoryOffset, const string& sectionName, uint32_t expectedSize)
				{
					const auto rva = get32(image, optional + directoryOffset);
					Assert::IsTrue(rva != 0, L"runtime data-directory RVA is present");
					Assert::AreEqual<uint32_t>(expectedSize, get32(image, optional + directoryOffset + 4), L"runtime data-directory size");
					Assert::IsTrue(rva >= get32(image, sectionHeader(image, sectionName) + 12), L"runtime directory points into the expected output section");
				};
			verifyDirectory(128, ".rsrc", 16);
			verifyDirectory(136, ".pdata", 12);
			verifyDirectory(184, ".rdata", 40);
			verifyDirectory(192, ".rdata", 0x70);
			const auto pdataHeader = sectionHeader(image, ".pdata");
			const auto pdataRaw = get32(image, pdataHeader + 20);
			Assert::AreEqual<uint32_t>(get32(image, sectionHeader(image, ".text") + 12), get32(image, pdataRaw), L"RUNTIME_FUNCTION begins at the linked code RVA");
			Assert::AreEqual<uint32_t>(get32(image, pdataRaw + 8), get32(image, sectionHeader(image, ".xdata") + 12), L"RUNTIME_FUNCTION retains its unwind-info RVA");
			const auto tlsRva = get32(image, optional + 184);
			const auto tlsRaw = rvaOffset(image, tlsRva);
			const auto tlsBase = get64(image, optional + 24);
			Assert::AreEqual<uint64_t>(tlsBase + get32(image, sectionHeader(image, ".tls") + 12), get64(image, tlsRaw), L"TLS template start VA is relocated");
			Assert::AreEqual<uint64_t>(tlsBase + get32(image, sectionHeader(image, ".tls") + 12) + 4, get64(image, tlsRaw + 8), L"TLS template end VA is relocated");
			assertHasBaseReloc(image, tlsRva, 10);
		}

		TEST_METHOD(MalformedExceptionDirectorySizeIsRejected)
		{
			Workspace workspace;
			const auto object = makeObject({ TestSection{ ".text",{ 0xc3 } }, TestSection{ ".pdata", Bytes(13, 0), 0x40000040 } }, { TestSymbol{ "main", 0, 1, 2,{} } });
			const auto result = linkObject(workspace, object);
			assertFailureContains(result, "exception directory size is not a multiple of RUNTIME_FUNCTION");
			assertNoOutput(workspace);
		}

		TEST_METHOD(UnsortedExceptionDirectoryIsRejected)
		{
			Workspace workspace;
			TestSection pdata{ ".pdata", Bytes(24, 0), 0x40000040 };
			pdata.relocations = { { 0, 1, 3 },{ 4, 3, 3 },{ 8, 4, 3 },{ 12, 0, 3 },{ 16, 2, 3 },{ 20, 4, 3 } };
			const auto object = makeObject({ TestSection{ ".text",{ 0xc3, 0xc3, 0xc3 } }, pdata, TestSection{ ".xdata",{ 1, 0, 0, 0 }, 0x40000040 } }, {
				TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "second", 2, 1, 2,{} }, TestSymbol{ "firstEnd", 1, 1, 2,{} },
				TestSymbol{ "secondEnd", 3, 1, 2,{} }, TestSymbol{ "unwindInfo", 0, 3, 2,{} }
				});
			const auto result = linkObject(workspace, object);
			assertFailureContains(result, "unsorted RUNTIME_FUNCTION entry");
			assertNoOutput(workspace);
		}

		TEST_METHOD(ShortResourceDirectoryIsRejected)
		{
			Workspace workspace;
			const auto object = makeObject({ TestSection{ ".text",{ 0xc3 } }, TestSection{ ".rsrc", Bytes(15, 0), 0x40000040 } }, { TestSymbol{ "main", 0, 1, 2,{} } });
			const auto result = linkObject(workspace, object);
			assertFailureContains(result, "resource directory is shorter than IMAGE_RESOURCE_DIRECTORY");
			assertNoOutput(workspace);
		}

		TEST_METHOD(DebugOptionWritesMatchingPdbAndRemapsCodeViewLines)
		{
			Workspace workspace;
			TestSection text{ ".text",{ 0xc3 }, 0x60000020 };
			Bytes types(4, 0);
			put32(types, 0, 4);
			Bytes modifier{ 8, 0, 1, 0x10, 0x74, 0, 0, 0, 0, 0 };
			types.insert(types.end(), modifier.begin(), modifier.end());
			TestSection debugTypes{ ".debug$T", types, 0x42000040 };
			Bytes debugSymbols(4, 0);
			put32(debugSymbols, 0, 4);
			Bytes publicSymbol(4, 0);
			put16(publicSymbol, 2, 0x110e);
			put32(publicSymbol, 4, 0);
			put32(publicSymbol, 8, 0);
			put16(publicSymbol, 12, 1);
			appendString(publicSymbol, "main");
			put16(publicSymbol, 0, static_cast<uint16_t>(publicSymbol.size() - 2));
			put32(debugSymbols, debugSymbols.size(), 0xF1);
			put32(debugSymbols, debugSymbols.size(), static_cast<uint32_t>(publicSymbol.size()));
			const auto symbolPayload = debugSymbols.size();
			debugSymbols.insert(debugSymbols.end(), publicSymbol.begin(), publicSymbol.end());
			debugSymbols.resize((debugSymbols.size() + 3) & ~size_t(3), 0);
			Bytes stringTable{ 0 };
			appendString(stringTable, "debug.cpp");
			put32(debugSymbols, debugSymbols.size(), 0xF3);
			put32(debugSymbols, debugSymbols.size(), static_cast<uint32_t>(stringTable.size()));
			debugSymbols.insert(debugSymbols.end(), stringTable.begin(), stringTable.end());
			debugSymbols.resize((debugSymbols.size() + 3) & ~size_t(3), 0);
			Bytes checksums(8, 0);
			put32(checksums, 0, 1);
			put32(debugSymbols, debugSymbols.size(), 0xF4);
			put32(debugSymbols, debugSymbols.size(), static_cast<uint32_t>(checksums.size()));
			debugSymbols.insert(debugSymbols.end(), checksums.begin(), checksums.end());
			Bytes lines(32, 0);
			put32(lines, 8, 1);
			put32(lines, 16, 1);
			put32(lines, 20, 20);
			put32(lines, 28, 1);
			put32(debugSymbols, debugSymbols.size(), 0xF2);
			put32(debugSymbols, debugSymbols.size(), static_cast<uint32_t>(lines.size()));
			const auto linePayload = debugSymbols.size();
			debugSymbols.insert(debugSymbols.end(), lines.begin(), lines.end());
			TestSection debugSymbolsSection{ ".debug$S", debugSymbols, 0x42000040 };
			debugSymbolsSection.relocations = { { static_cast<uint32_t>(symbolPayload + 8), 0, 0x000b },{ static_cast<uint32_t>(symbolPayload + 12), 0, 0x000a },{ static_cast<uint32_t>(linePayload), 0, 0x000b },{ static_cast<uint32_t>(linePayload + 4), 0, 0x000a } };
			const auto object = makeObject({ text, debugTypes, debugSymbolsSection }, { TestSymbol{ "main", 0, 1, 2,{} } });
			auto args = outputArgs(workspace);
			args.insert(args.begin() + 1, L"/DEBUG");
			args.push_back(workspace.file(L"debug.obj").wstring());
			workspace.write(L"debug.obj", object);
			assertSuccess(runLinker(workspace, args));
			const auto image = workspace.read(L"linked.exe");
			const auto optional = optionalHeader(image);
			const auto debugRva = get32(image, optional + 160);
			Assert::IsTrue(debugRva != 0, L"PE debug data directory is present");
			Assert::AreEqual<uint32_t>(28, get32(image, optional + 164), L"PE debug directory contains one RSDS entry");
			const auto directory = rvaOffset(image, debugRva);
			Assert::AreEqual<uint32_t>(2, get32(image, directory + 12), L"debug entry identifies CodeView data");
			const auto recordOffset = get32(image, directory + 24);
			Assert::AreEqual<uint32_t>(0x53445352, get32(image, recordOffset), L"CodeView record has the RSDS signature");
			const auto pdb = workspace.read(L"linked.pdb");
			constexpr array<uint8_t, 24> msfMagic{ 'M','i','c','r','o','s','o','f','t',' ','C','/','C','+','+',' ','M','S','F',' ','7','.','0','0' };
			Assert::IsTrue(pdb.size() > 3 * 4096 && equal(msfMagic.begin(), msfMagic.end(), pdb.begin()), L"/DEBUG writes an MSF 7.0 PDB file");
			const auto pdbInfoStream = 3 * 4096;
			Assert::IsTrue(equal(pdb.begin() + pdbInfoStream + 12, pdb.begin() + pdbInfoStream + 28, image.begin() + recordOffset + 4), L"PDB GUID matches the PE RSDS record");
			Assert::AreEqual<uint32_t>(get32(pdb, pdbInfoStream + 8), get32(image, recordOffset + 20), L"PDB age matches the PE RSDS record");

			// Interoperability: a Windows debugger-engine reader must resolve the public symbol, source file, and remapped line.
			const auto dbghelp = LoadLibraryW(L"C:\\Windows\\System32\\dbghelp.dll");
			Assert::IsNotNull(dbghelp, L"dbghelp.dll is available");
			using SymSetOptionsFn = DWORD(WINAPI*)(DWORD);
			using SymInitializeWFn = BOOL(WINAPI*)(HANDLE, PCWSTR, BOOL);
			using SymLoadModuleExWFn = DWORD64(WINAPI*)(HANDLE, HANDLE, PCWSTR, PCWSTR, DWORD64, DWORD, PVOID, DWORD);
			using SymFromNameFn = BOOL(WINAPI*)(HANDLE, PCSTR, PSYMBOL_INFO);
			using SymGetLineFn = BOOL(WINAPI*)(HANDLE, DWORD64, PDWORD, PIMAGEHLP_LINE64);
			using SymCleanupFn = BOOL(WINAPI*)(HANDLE);
			const auto symSetOptions = reinterpret_cast<SymSetOptionsFn>(GetProcAddress(dbghelp, "SymSetOptions"));
			const auto symInitialize = reinterpret_cast<SymInitializeWFn>(GetProcAddress(dbghelp, "SymInitializeW"));
			const auto symLoadModule = reinterpret_cast<SymLoadModuleExWFn>(GetProcAddress(dbghelp, "SymLoadModuleExW"));
			const auto symFromName = reinterpret_cast<SymFromNameFn>(GetProcAddress(dbghelp, "SymFromName"));
			const auto symGetLine = reinterpret_cast<SymGetLineFn>(GetProcAddress(dbghelp, "SymGetLineFromAddr64"));
			const auto symCleanup = reinterpret_cast<SymCleanupFn>(GetProcAddress(dbghelp, "SymCleanup"));
			Assert::IsTrue(symSetOptions && symInitialize && symLoadModule && symFromName && symGetLine && symCleanup, L"dbghelp exports the symbol APIs");
			const auto process = reinterpret_cast<HANDLE>(static_cast<uintptr_t>(0x4a574c32));
			symSetOptions(0x00000002 | 0x00000010); // SYMOPT_UNDNAME | SYMOPT_LOAD_LINES
			const auto imageBase = get64(image, optional + 24);
			DWORD64 loaded = 0;
			// Freshly written files can be briefly held by a scanner, which makes the first dbghelp load fail intermittently.
			for (int attempt = 0; attempt < 10 && loaded == 0; ++attempt)
			{
				if (attempt != 0) Sleep(100);
				Assert::IsTrue(symInitialize(process, workspace.file(L"").c_str(), FALSE) != FALSE, L"dbghelp initializes");
				loaded = symLoadModule(process, nullptr, workspace.file(L"linked.exe").c_str(), nullptr, imageBase, get32(image, optional + 56), nullptr, 0);
				if (loaded == 0) symCleanup(process);
			}
			Assert::IsTrue(loaded != 0, L"dbghelp loads the linked image and PDB");
			alignas(SYMBOL_INFO) BYTE symbolBuffer[sizeof(SYMBOL_INFO) + 64]{};
			auto* symbol = reinterpret_cast<PSYMBOL_INFO>(symbolBuffer);
			symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
			symbol->MaxNameLen = 63;
			const auto found = symFromName(process, "main", symbol) != FALSE;
			IMAGEHLP_LINE64 line{};
			line.SizeOfStruct = sizeof(line);
			DWORD displacement = 0;
			const auto hasLine = found && symGetLine(process, symbol->Address, &displacement, &line) != FALSE;
			const string fileName = hasLine ? string(line.FileName) : string();
			symCleanup(process);
			Assert::IsTrue(found, L"PDB public symbols resolve main");
			Assert::AreEqual<uint64_t>(imageBase + get32(image, sectionHeader(image, ".text") + 12), symbol->Address, L"PDB symbol address is the linked RVA");
			Assert::IsTrue(hasLine, L"PDB line records resolve the entry address");
			Assert::AreEqual<DWORD>(1, line.LineNumber, L"line number is preserved");
			Assert::IsTrue(fileName.find("debug.cpp") != string::npos, L"source file name is preserved");
		}

		TEST_METHOD(RealMsvcCodeViewSourceAndLineMatchMicrosoftLink)
		{
			fs::path toolDirectory;
			error_code ignored;
			wchar_t programFiles[MAX_PATH]{};
			GetEnvironmentVariableW(L"ProgramFiles", programFiles, MAX_PATH);
			for (const auto& entry : fs::recursive_directory_iterator(fs::path(programFiles) / L"Microsoft Visual Studio", fs::directory_options::skip_permission_denied, ignored))
			{
				if (entry.path().filename() == L"cl.exe" && entry.path().parent_path().filename() == L"x64" && entry.path().parent_path().parent_path().filename() == L"Hostx64" && fs::exists(entry.path().parent_path() / L"link.exe")) toolDirectory = entry.path().parent_path();
			}
			if (toolDirectory.empty()) { Logger::WriteMessage("MSVC x64 tools not found; real CodeView comparison skipped"); return; }
			Workspace workspace;
			workspace.writeText(L"source.c", L"int main(void) {\n return 42;\n}\n");
			assertSuccess(runExecutable(workspace, toolDirectory / L"cl.exe", { L"/nologo", L"/c", L"/Z7", L"/Od", L"/GS-", L"/Fosource.obj", L"source.c" }));
			assertSuccess(runLinker(workspace, { L"/debug", L"/nodefaultlib", L"/entry:main", L"/out:jw2.exe", L"source.obj" }));
			assertSuccess(runExecutable(workspace, toolDirectory / L"link.exe", { L"/nologo", L"/debug", L"/incremental:no", L"/nodefaultlib", L"/subsystem:console", L"/entry:main", L"/out:msvc.exe", L"source.obj" }));
			const auto dbghelp = LoadLibraryW(L"C:\\Windows\\System32\\dbghelp.dll");
			Assert::IsNotNull(dbghelp);
			using SetOptions = DWORD(WINAPI*)(DWORD);
			using Initialize = BOOL(WINAPI*)(HANDLE, PCWSTR, BOOL);
			using LoadModule = DWORD64(WINAPI*)(HANDLE, HANDLE, PCWSTR, PCWSTR, DWORD64, DWORD, PVOID, DWORD);
			using FromName = BOOL(WINAPI*)(HANDLE, PCSTR, PSYMBOL_INFO);
			using GetLine = BOOL(WINAPI*)(HANDLE, DWORD64, PDWORD, PIMAGEHLP_LINE64);
			using Cleanup = BOOL(WINAPI*)(HANDLE);
			const auto setOptions = reinterpret_cast<SetOptions>(GetProcAddress(dbghelp, "SymSetOptions"));
			const auto initialize = reinterpret_cast<Initialize>(GetProcAddress(dbghelp, "SymInitializeW"));
			const auto loadModule = reinterpret_cast<LoadModule>(GetProcAddress(dbghelp, "SymLoadModuleExW"));
			const auto fromName = reinterpret_cast<FromName>(GetProcAddress(dbghelp, "SymFromName"));
			const auto getLine = reinterpret_cast<GetLine>(GetProcAddress(dbghelp, "SymGetLineFromAddr64"));
			const auto cleanup = reinterpret_cast<Cleanup>(GetProcAddress(dbghelp, "SymCleanup"));
			Assert::IsTrue(setOptions && initialize && loadModule && fromName && getLine && cleanup);
			setOptions(0x00000002 | 0x00000010);
			for (const auto* name : { L"jw2.exe", L"msvc.exe" })
			{
				const auto image = workspace.read(name);
				const auto optional = optionalHeader(image);
				const auto base = get64(image, optional + 24);
				const auto process = reinterpret_cast<HANDLE>(static_cast<uintptr_t>(0x4a574c33));
				DWORD64 loaded = 0;
				for (int attempt = 0; attempt < 10 && loaded == 0; ++attempt)
				{
					if (attempt) Sleep(100);
					Assert::IsTrue(initialize(process, workspace.path.c_str(), FALSE) != FALSE);
					loaded = loadModule(process, nullptr, workspace.file(name).c_str(), nullptr, base, get32(image, optional + 56), nullptr, 0);
					if (!loaded) cleanup(process);
				}
				Assert::IsTrue(loaded != 0, L"dbghelp loads image and PDB");
				alignas(SYMBOL_INFO) BYTE buffer[sizeof(SYMBOL_INFO) + 64]{};
				auto* symbol = reinterpret_cast<PSYMBOL_INFO>(buffer);
				symbol->SizeOfStruct = sizeof(SYMBOL_INFO);
				symbol->MaxNameLen = 63;
				const auto found = fromName(process, "main", symbol) != FALSE;
				IMAGEHLP_LINE64 line{};
				line.SizeOfStruct = sizeof(line);
				DWORD displacement = 0;
				const auto hasLine = found && getLine(process, symbol->Address, &displacement, &line) != FALSE;
				const string filename = hasLine ? line.FileName : "";
				cleanup(process);
				Assert::IsTrue(found && hasLine, L"dbghelp resolves main and its source line");
				Assert::AreEqual<uint64_t>(base + get32(image, optional + 16), symbol->Address, L"public symbol is at the entry point");
				Assert::AreEqual<DWORD>(1, line.LineNumber, L"source line matches compiler input");
				Assert::IsTrue(filename.find("source.c") != string::npos, L"source path is preserved");
			}
			FreeLibrary(dbghelp);
		}

		TEST_METHOD(JwlinkDebugFormatsAndModifiersHaveDistinctArtifacts)
		{
			Workspace workspace;
			workspace.writeText(L"source.asm", L"option casemap:none\n.code\npublic main\nmain proc\n mov eax, 42\n ret\nmain endp\nend\n");
			assertSuccess(runExecutable(workspace, toolPath(L"JWasm.exe"), { L"-win64", L"-Zi", L"-Fosource.obj", L"source.asm" }));
			auto link = [&](const vector<wstring>& debug, const wstring& output)
				{
					vector<wstring> args{ L"format", L"win", L"pe", L"debug" };
					args.insert(args.end(), debug.begin(), debug.end());
					args.insert(args.end(), { L"option", L"quiet,start=main", L"file", L"source.obj", L"name", output });
					assertSuccess(runExecutable(workspace, jwlinkPath(), args));
					return workspace.read(output);
				};
			auto subsections = [&](const Bytes& image)
				{
					const auto optional = optionalHeader(image);
					const auto debugRva = get32(image, optional + 160);
					const auto size = get32(image, optional + 164);
					Assert::IsTrue(debugRva != 0 && size >= 56, L"JWlink CodeView has two PE debug entries");
					const auto entries = rvaOffset(image, debugRva);
					Assert::AreEqual<uint32_t>(4, get32(image, entries + 12), L"first entry is IMAGE_DEBUG_TYPE_MISC");
					Assert::AreEqual<uint32_t>(2, get32(image, entries + 28 + 12), L"second entry is IMAGE_DEBUG_TYPE_CODEVIEW");
					const auto offset = get32(image, entries + 28 + 24);
					const auto length = get32(image, entries + 28 + 16);
					Assert::IsTrue(offset <= image.size() && length <= image.size() - offset && length >= 24, L"NB05 data is within the image");
					Assert::AreEqual<uint32_t>(0x3530424e, get32(image, offset), L"JWlink emits NB05 rather than RSDS");
					const auto directory = static_cast<size_t>(get32(image, offset + 4));
					Assert::IsTrue(directory <= length && 16 <= length - directory, L"NB05 directory header is in bounds");
					const auto headSize = get16(image, offset + directory);
					const auto entrySize = get16(image, offset + directory + 2);
					const auto count = get32(image, offset + directory + 4);
					Assert::IsTrue(headSize >= 16 && entrySize >= 12 && headSize <= length - directory && count <= (length - directory - headSize) / entrySize, L"NB05 directory entries are in bounds");
					vector<uint16_t> kinds;
					for (uint32_t i = 0; i < count; ++i)
					{
						const auto entry = offset + directory + headSize + static_cast<size_t>(i) * entrySize;
						const auto relative = get32(image, entry + 4);
						const auto bytes = get32(image, entry + 8);
						Assert::IsTrue(relative <= length && bytes <= length - relative, L"NB05 subsection is in bounds");
						const auto kind = get16(image, entry);
						const auto start = offset + relative;
						if (kind == 0x123)
						{
							const string publicRecords(image.begin() + start, image.begin() + start + bytes);
							Assert::IsTrue(publicRecords.find("main") != string::npos, L"NB05 public symbol records include main");
						}
						if (kind == 0x127)
						{
							Assert::IsTrue(bytes >= 20, L"NB05 source module header is present");
							const auto file = get32(image, start + 4);
							Assert::IsTrue(file <= bytes && bytes - file >= 17, L"NB05 source file record is in bounds");
							const auto nameLength = image[start + file + 16];
							Assert::IsTrue(nameLength <= bytes - file - 17, L"NB05 source name is in bounds");
							const string source(image.begin() + start + file + 17, image.begin() + start + file + 17 + nameLength);
							Assert::IsTrue(source.find("source.asm") != string::npos, L"NB05 source module names the source file");
							const auto mapping = get32(image, start + file + 4);
							Assert::IsTrue(mapping <= bytes && bytes - mapping >= 4, L"NB05 source line table is in bounds");
							const auto pairs = get16(image, start + mapping + 2);
							Assert::IsTrue(pairs != 0 && pairs <= (bytes - mapping - 4) / 6, L"NB05 source line pairs are in bounds");
							Assert::AreEqual<uint16_t>(4, get16(image, start + mapping + 4 + pairs * 4), L"first JWasm source line is preserved");
						}
						kinds.push_back(kind);
					}
					return kinds;
				};
			const auto all = subsections(link({ L"codeview" }, L"all.exe"));
			const auto lines = subsections(link({ L"codeview", L"lines" }, L"lines.exe"));
			const auto typesLocals = subsections(link({ L"codeview", L"types,locals" }, L"types.exe"));
			auto has = [](const vector<uint16_t>& kinds, uint16_t kind) { return find(kinds.begin(), kinds.end(), kind) != kinds.end(); };
			Assert::IsTrue(has(all, 0x121) && has(all, 0x124) && has(all, 0x127), L"all contains type, local-symbol and source-line subsections");
			Assert::IsTrue(has(lines, 0x127) && !has(lines, 0x121) && !has(lines, 0x124), L"lines omits types and locals");
			Assert::IsTrue(!has(typesLocals, 0x127) && has(typesLocals, 0x121) && has(typesLocals, 0x124), L"types and locals omit source lines");
			for (const auto& mode : { vector<wstring>{ L"dwarf" }, vector<wstring>{ L"watcom" }, vector<wstring>{ L"all" } })
			{
				const auto image = link(mode, L"other.exe");
				Assert::AreEqual<uint32_t>(0, get32(image, optionalHeader(image) + 160), L"non-CodeView output has no PE CodeView debug directory in this fixture");
			}
			assertFailureContains(runLinker(workspace, { L"debug", L"codeview", L"lines", L"file", L"source.obj" }), "unsupported DEBUG modifier 'lines'");
			assertFailureContains(runLinker(workspace, { L"debug", L"codeview", L"types,locals", L"file", L"source.obj" }), "unsupported DEBUG modifier 'types'");
			assertFailureContains(runLinker(workspace, { L"debug", L"locals", L"file", L"source.obj" }), "unsupported DEBUG modifier 'locals'");
			assertFailureContains(runLinker(workspace, { L"debug", L"watcom", L"file", L"source.obj" }), "unsupported DEBUG format 'watcom'");
			assertFailureContains(runLinker(workspace, { L"debug", L"dwarf", L"file", L"source.obj" }), "unsupported DEBUG format 'dwarf'");
			assertFailureContains(runLinker(workspace, { L"/debug", L"/entry:main", L"/out:new.exe", L"source.obj" }), "legacy CodeView .debug$T signature 1");
			assertSuccess(runLinker(workspace, { L"/entry:main", L"/out:nodebug.exe", L"source.obj" }));
			Assert::AreEqual<DWORD>(42, runExecutable(workspace, workspace.file(L"nodebug.exe"), {}).exitCode, L"legacy debug input can still be linked without debug output");
			workspace.write(L"plain.obj", makeEntryObject());
			assertSuccess(runLinker(workspace, { L"debug", L"all", L"option", L"start=main", L"file", L"plain.obj", L"name", L"extension.exe" }));
			const auto extension = workspace.read(L"extension.exe");
			const auto directory = rvaOffset(extension, get32(extension, optionalHeader(extension) + 160));
			Assert::AreEqual<uint32_t>(2, get32(extension, directory + 12), L"JWlink2 debug all extension emits CodeView");
			Assert::AreEqual<uint32_t>(0x53445352, get32(extension, get32(extension, directory + 24)), L"JWlink2 emits RSDS rather than JWlink's default DWARF");
			Assert::IsTrue(fs::exists(workspace.file(L"extension.pdb")), L"JWlink2 debug all extension writes a PDB");
			workspace.write(L"legacy-symbols.obj", makeObject({ TestSection{ ".text",{ 0xc3 } }, TestSection{ ".debug$S",{ 1, 0, 0, 0 }, 0x42000040 } }, { TestSymbol{ "main", 0, 1, 2,{} } }));
			assertFailureContains(runLinker(workspace, { L"/debug", L"/entry:main", L"legacy-symbols.obj" }), "legacy CodeView .debug$S signature 1");
		}

		TEST_METHOD(DebugOptionAcceptsMsvcTypeAndIdRecordsAndRejectsUnknownLeaves)
		{
			auto typeStream = [](const vector<Bytes>& records)
				{
					Bytes data(4, 0);
					put32(data, 0, 4);
					for (const auto& record : records) data.insert(data.end(), record.begin(), record.end());
					return data;
				};
			auto record = [](uint16_t leaf, const Bytes& body)
				{
					Bytes result(4, 0);
					put16(result, 2, leaf);
					result.insert(result.end(), body.begin(), body.end());
					while (result.size() % 4 != 0) result.push_back(static_cast<uint8_t>(0xF0 + 4 - result.size() % 4));
					put16(result, 0, static_cast<uint16_t>(result.size() - 2));
					return result;
				};
			auto link = [&](Workspace& workspace, const Bytes& types, const wstring& pdbName)
				{
					const auto object = makeObject({ TestSection{ ".text",{ 0xc3 }, 0x60000020 }, TestSection{ ".debug$T", types, 0x42000040 } }, { TestSymbol{ "main", 0, 1, 2,{} } });
					workspace.write(L"types.obj", object);
					auto args = outputArgs(workspace);
					args.insert(args.begin() + 1, L"/PDB:" + workspace.file(pdbName).wstring());
					args.push_back(workspace.file(L"types.obj").wstring());
					return runLinker(workspace, args);
				};

			Bytes field;
			put16(field, field.size(), 0x150D); put16(field, field.size(), 3); put32(field, field.size(), 0x74); put16(field, field.size(), 0); appendString(field, "x");
			Bytes structure;
			put16(structure, structure.size(), 1); put16(structure, structure.size(), 0); put32(structure, structure.size(), 0x1000); put32(structure, structure.size(), 0); put32(structure, structure.size(), 0);
			put16(structure, structure.size(), 4); appendString(structure, "S");
			Bytes stringId;
			put32(stringId, stringId.size(), 0); appendString(stringId, "a.c");
			Bytes substrings;
			put32(substrings, substrings.size(), 1); put32(substrings, substrings.size(), 0x1002);
			Bytes buildInfo;
			put16(buildInfo, buildInfo.size(), 1); put32(buildInfo, buildInfo.size(), 0x1002);
			Workspace accepted;
			assertSuccess(link(accepted, typeStream({ record(0x1203, field), record(0x1505, structure), record(0x1605, stringId), record(0x1604, substrings), record(0x1603, buildInfo) }), L"types.pdb"));
			Assert::IsTrue(fs::exists(accepted.file(L"types.pdb")), L"/PDB writes the requested PDB path");

			Workspace rejected;
			assertFailureContains(link(rejected, typeStream({ record(0x1700, Bytes(4, 0)) }), L"bad.pdb"), "unsupported CodeView type leaf");
		}

		TEST_METHOD(ValidAmd64ObjectProducesPe32PlusWithDefaultBaseAndEntry)
		{
			Workspace workspace;
			assertSuccess(linkObject(workspace, makeEntryObject()));
			const auto image = workspace.read(L"linked.exe");
			const auto optional = optionalHeader(image);
			Assert::AreEqual<uint16_t>(0x8664, get16(image, 0x84), L"AMD64 machine");
			Assert::AreEqual<uint64_t>(0x140000000, get64(image, optional + 24), L"default image base");
			Assert::AreEqual<uint32_t>(0x1000, get32(image, optional + 16), L"default entry point RVA");
			Assert::AreEqual<uint16_t>(3, get16(image, optional + 68), L"default console subsystem");
			Assert::IsTrue((get16(image, 0x96) & 0x2000) == 0, L"supported output is a PE executable, not a DLL");
		}


		TEST_METHOD(DllExportsGenerateRoundTrippableImportLibrary)
		{
			Workspace workspace;
			workspace.write(L"dll.obj", makeObject({ TestSection{ ".text",{ 0xc3 } } }, { TestSymbol{ "api", 0, 1, 2,{} } }));
			assertSuccess(runLinker(workspace, { L"/DLL", L"/EXPORT:api", L"/IMPLIB:api-imports.lib", workspace.file(L"dll.obj").wstring() }));
			const auto dll = workspace.read(L"a.dll");
			const auto pe = peHeader(dll);
			const auto optional = optionalHeader(dll);
			Assert::IsTrue((get16(dll, pe + 22) & 0x2000) != 0, L"DLL characteristic is set");
			Assert::AreEqual<uint32_t>(0, get32(dll, optional + 16), L"DLL without a startup routine has no entry point");
			const auto exportRva = get32(dll, optional + 112);
			Assert::IsTrue(exportRva != 0, L"export directory is present");
			const auto exportDirectory = rvaOffset(dll, exportRva);
			Assert::IsTrue(cStringAtRva(dll, get32(dll, exportDirectory + 12)) == "a.dll", L"export directory module name");
			Assert::IsTrue(fs::exists(workspace.file(L"api-imports.lib")), L"/IMPLIB selects the generated import-library path");

			TestSection clientText{ ".text", Bytes(16, 0) };
			clientText.relocations = { { 0, 1, 4 },{ 8, 2, 4 } };
			workspace.write(L"client.obj", makeObject({ clientText }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "api", 0, 0, 2,{} }, TestSymbol{ "__imp_api", 0, 0, 2,{} } }));
			assertSuccess(runLinker(workspace, { L"/OUT:" + workspace.file(L"client.exe").wstring(), L"/ENTRY:main", workspace.file(L"client.obj").wstring(), workspace.file(L"api-imports.lib").wstring() }));
			const auto client = workspace.read(L"client.exe");
			const auto clientTextHeader = sectionHeader(client, ".text");
			const auto textRaw = get32(client, clientTextHeader + 20);
			const auto textRva = get32(client, clientTextHeader + 12);
			const auto apiThunkRva = static_cast<uint32_t>(static_cast<int64_t>(textRva) + 4 + static_cast<int32_t>(get32(client, textRaw)));
			const auto apiOffset = rvaOffset(client, apiThunkRva);
			Assert::AreEqual<uint8_t>(0xff, client[apiOffset], L"import library provides a callable function thunk");
			const auto importRva = get32(client, optionalHeader(client) + 120);
			Assert::IsTrue(importRva != 0, L"round-trip link includes import descriptors");
			const auto descriptor = rvaOffset(client, importRva);
			Assert::IsTrue(cStringAtRva(client, get32(client, descriptor + 12)) == "a.dll", L"generated import object names its DLL");
		}

		TEST_METHOD(DefinitionFileAndDllEntryPointOptionsAreHonored)
		{
			Workspace workspace;
			TestSection text{ ".text",{ 0xc3, 0xc3 } };
			TestSection data{ ".data",{ 0x5a }, 0xc0000040 };
			workspace.write(L"dll.obj", makeObject({ text, data }, {
				TestSymbol{ "DllMainCRTStartup", 0, 1, 2,{} }, TestSymbol{ "internalFn", 1, 1, 2,{} }, TestSymbol{ "globalData", 0, 2, 2,{} }
				}));
			workspace.writeText(L"module.def", L"LIBRARY fixture\nEXPORTS\n api=internalFn @5\n table=globalData @8 DATA\n ordinal=internalFn @11 NONAME\n hidden=internalFn PRIVATE\n");
			assertSuccess(runLinker(workspace, { L"/DLL", L"/DEF:" + workspace.file(L"module.def").wstring(), L"/OUT:" + workspace.file(L"custom.dll").wstring(), workspace.file(L"dll.obj").wstring() }));
			const auto dll = workspace.read(L"custom.dll");
			const auto optional = optionalHeader(dll);
			const auto entryRva = get32(dll, optional + 16);
			Assert::AreEqual<uint32_t>(get32(dll, sectionHeader(dll, ".text") + 12), entryRva, L"DLL defaults to DllMainCRTStartup when present");
			const auto exportDirectory = rvaOffset(dll, get32(dll, optional + 112));
			Assert::IsTrue(cStringAtRva(dll, get32(dll, exportDirectory + 12)) == "fixture.dll", L".def LIBRARY name controls the export module name");
			Assert::AreEqual<uint32_t>(11, get32(dll, exportDirectory + 20), L"export ordinal table spans assigned ordinals");
			Assert::AreEqual<uint32_t>(3, get32(dll, exportDirectory + 24), L"named exports include DATA and PRIVATE but omit NONAME");
			Assert::IsTrue(fs::exists(workspace.file(L"custom.lib")), L".def DLL emits the default import library");

			TestSection clientText{ ".text", Bytes(20, 0) };
			clientText.relocations = { { 0, 1, 4 },{ 8, 2, 4 },{ 16, 3, 4 } };
			workspace.write(L"client.obj", makeObject({ clientText }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "api", 0, 0, 2,{} }, TestSymbol{ "__imp_table", 0, 0, 2,{} }, TestSymbol{ "ordinal", 0, 0, 2,{} } }));
			assertSuccess(runLinker(workspace, { L"/OUT:" + workspace.file(L"client.exe").wstring(), L"/ENTRY:main", workspace.file(L"client.obj").wstring(), workspace.file(L"custom.lib").wstring() }));
			const auto client = workspace.read(L"client.exe");
			const auto importRva = get32(client, optionalHeader(client) + 120);
			const auto descriptor = rvaOffset(client, importRva);
			Assert::IsTrue(cStringAtRva(client, get32(client, descriptor + 12)) == "fixture.dll", L".def module name is carried into generated imports");
			const auto clientTextHeader = sectionHeader(client, ".text");
			const auto textRaw = get32(client, clientTextHeader + 20);
			const auto textRva = get32(client, clientTextHeader + 12);
			const auto dataIatRva = static_cast<uint32_t>(static_cast<int64_t>(textRva) + 12 + static_cast<int32_t>(get32(client, textRaw + 8)));
			const auto lookupOffset = rvaOffset(client, get32(client, descriptor));
			const auto iatRva = get32(client, descriptor + 16);
			bool foundDataImport = false;
			for (size_t index = 0;; ++index)
			{
				const auto nameRva = get64(client, lookupOffset + index * 8);
				if (nameRva == 0) break;
				if ((nameRva & (uint64_t{ 1 } << 63)) != 0) continue;
				const auto nameOffset = rvaOffset(client, static_cast<uint32_t>(nameRva)) + 2;
				const string importedName(reinterpret_cast<const char*>(client.data() + nameOffset));
				if (importedName == "table")
				{
					Assert::AreEqual<uint32_t>(iatRva + static_cast<uint32_t>(index * 8), dataIatRva, L"DATA import resolves exactly to its generated IAT slot");
					foundDataImport = true;
				}
			}
			Assert::IsTrue(foundDataImport, L"DATA import is present in the generated lookup table");
			const auto ordinalLookup = rvaOffset(client, get32(client, descriptor));
			bool foundOrdinalImport = false;
			for (size_t index = 0;; ++index)
			{
				const auto thunk = get64(client, ordinalLookup + index * 8);
				if (thunk == 0) break;
				if ((thunk & (uint64_t{ 1 } << 63)) != 0 && (thunk & 0xffff) == 11) foundOrdinalImport = true;
			}
			Assert::IsTrue(foundOrdinalImport, L"NONAME export is imported by its assigned ordinal");
			TestSection privateText{ ".text", Bytes(8, 0) };
			privateText.relocations = { { 0, 1, 4 } };
			workspace.write(L"private-client.obj", makeObject({ privateText }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "hidden", 0, 0, 2,{} } }));
			assertFailureContains(runLinker(workspace, { L"/OUT:" + workspace.file(L"private-client.exe").wstring(), L"/ENTRY:main", workspace.file(L"private-client.obj").wstring(), workspace.file(L"custom.lib").wstring() }), "unresolved external symbol 'hidden'");
		}

		TEST_METHOD(DllNoEntryAndInvalidImportLibraryRequestsAreHandled)
		{
			Workspace workspace;
			workspace.write(L"dll.obj", makeObject({ TestSection{ ".text",{ 0xc3 } } }, { TestSymbol{ "api", 0, 1, 2,{} } }));
			assertSuccess(runLinker(workspace, { L"/DLL", L"/NOENTRY", L"/EXPORT:api", L"/OUT:" + workspace.file(L"noentry.dll").wstring(), workspace.file(L"dll.obj").wstring() }));
			const auto noEntryImage = workspace.read(L"noentry.dll");
			Assert::AreEqual<uint32_t>(0, get32(noEntryImage, optionalHeader(noEntryImage) + 16), L"/NOENTRY leaves the DLL entry RVA zero");
			assertSuccess(runLinker(workspace, { L"/DLL", L"/ENTRY:api", L"/EXPORT:api", L"/OUT:" + workspace.file(L"explicit.dll").wstring(), workspace.file(L"dll.obj").wstring() }));
			const auto explicitEntryImage = workspace.read(L"explicit.dll");
			Assert::AreEqual<uint32_t>(get32(explicitEntryImage, sectionHeader(explicitEntryImage, ".text") + 12), get32(explicitEntryImage, optionalHeader(explicitEntryImage) + 16), L"/ENTRY selects an explicit DLL entry point");
			Workspace invalidWorkspace;
			invalidWorkspace.write(L"plain.obj", makeEntryObject());
			assertFailureContains(runLinker(invalidWorkspace, { L"/DLL", L"/IMPLIB:requested.lib", L"/OUT:" + invalidWorkspace.file(L"empty.dll").wstring(), invalidWorkspace.file(L"plain.obj").wstring() }), "DLL has no importable exports");
			assertNoOutput(invalidWorkspace, L"empty.dll");
			Assert::IsFalse(fs::exists(invalidWorkspace.file(L"requested.lib")), L"failed DLL link does not create an import library");
			assertFailureContains(runLinker(invalidWorkspace, { L"/DLL", L"/NOENTRY", L"/ENTRY:main", invalidWorkspace.file(L"plain.obj").wstring() }), "/NOENTRY is valid only for a DLL without /ENTRY");
			assertFailureContains(runLinker(invalidWorkspace, { L"/DLL", L"/EXPORT:missing", invalidWorkspace.file(L"plain.obj").wstring() }), "unresolved external symbol 'missing'");
			assertFailureContains(runLinker(invalidWorkspace, { L"/DLL", L"/EXPORT:main", L"/EXPORT:main", invalidWorkspace.file(L"plain.obj").wstring() }), "duplicate export name 'main'");
			assertFailureContains(runLinker(invalidWorkspace, { L"/DLL", L"/EXPORT:main", L"/OUT:" + invalidWorkspace.file(L"same.dll").wstring(), L"/IMPLIB:" + invalidWorkspace.file(L"same.dll").wstring(), invalidWorkspace.file(L"plain.obj").wstring() }), "import-library output path must differ");
		}

		TEST_METHOD(LinkedImagesSatisfyPeLayoutInvariants)
		{
			Workspace workspace;
			TestSection data{ ".data", Bytes(16, 0), 0xc0000040 };
			data.relocations = { { 0, 0, 1 } };
			TestSection text{ ".text",{ 0xc3 } };
			const auto object = makeObject({ data, text }, { TestSymbol{ "main", 0, 2, 2,{} } });
			assertSuccess(linkObject(workspace, object));
			validatePeLayout(workspace.read(L"linked.exe"));

			auto args = outputArgs(workspace, L"aligned.exe");
			args.insert(args.begin() + 1, L"/ALIGN:8192");
			args.push_back(workspace.file(L"input.obj").wstring());
			workspace.write(L"input.obj", object);
			assertSuccess(runLinker(workspace, args));
			validatePeLayout(workspace.read(L"aligned.exe"));

			workspace.write(L"dll.obj", makeObject({ TestSection{ ".text",{ 0xc3 } } }, { TestSymbol{ "api", 0, 1, 2,{} } }));
			assertSuccess(runLinker(workspace, { L"/DLL", L"/NOENTRY", L"/EXPORT:api", L"/OUT:" + workspace.file(L"layout.dll").wstring(), workspace.file(L"dll.obj").wstring() }));
			validatePeLayout(workspace.read(L"layout.dll"));
		}

		TEST_METHOD(StaticComdatSectionsWithoutExternalSymbolsAreKept)
		{
			Workspace workspace;
			auto staticComdat = [](uint8_t marker)
				{
					TestSection section{ ".text$mn", { marker }, 0x60001020 };
					TestSymbol sectionSymbol{ ".text$mn", 0, 1, 3,{ sectionAux(1, 0, 2) } };
					return makeObject({ section }, { sectionSymbol });
				};
			workspace.write(L"entry.obj", makeEntryObject());
			workspace.write(L"first.obj", staticComdat(0x11));
			workspace.write(L"second.obj", staticComdat(0x22));
			auto args = outputArgs(workspace);
			for (const auto& file : { L"entry.obj", L"first.obj", L"second.obj" }) args.push_back(workspace.file(file).wstring());
			assertSuccess(runLinker(workspace, args));
			const auto image = workspace.read(L"linked.exe");
			const auto text = sectionHeader(image, ".text");
			const auto raw = get32(image, text + 20);
			const auto end = raw + get32(image, text + 16);
			Assert::IsTrue(find(image.begin() + raw, image.begin() + end, 0x11) != image.begin() + end, L"first static COMDAT is kept");
			Assert::IsTrue(find(image.begin() + raw, image.begin() + end, 0x22) != image.begin() + end, L"static COMDATs from different objects never merge");
		}

		TEST_METHOD(DllIsRebasedByTheWindowsLoaderAndRelocationsAreApplied)
		{
			Workspace workspace;
			// get: mov rax,[rip+slot]; ret    where: lea rax,[rip+slot]; ret
			TestSection text{ ".text",{ 0x48, 0x8b, 0x05, 0, 0, 0, 0, 0xc3, 0x48, 0x8d, 0x05, 0, 0, 0, 0, 0xc3 } };
			text.relocations = { { 3, 2, 4 },{ 11, 2, 4 } };
			TestSection data{ ".data", Bytes(8, 0), 0xc0000040 };
			data.relocations = { { 0, 2, 1 } };
			workspace.write(L"dll.obj", makeObject({ text, data }, { TestSymbol{ "get", 0, 1, 2,{} }, TestSymbol{ "where", 8, 1, 2,{} }, TestSymbol{ "slot", 0, 2, 2,{} } }));
			for (const auto* name : { L"one.dll", L"two.dll" })
			{
				assertSuccess(runLinker(workspace, { L"/DLL", L"/NOENTRY", L"/BASE:0x180000000", L"/DYNAMICBASE", L"/EXPORT:get", L"/EXPORT:where", L"/OUT:" + workspace.file(name).wstring(), workspace.file(L"dll.obj").wstring() }));
			}
			const auto image = workspace.read(L"one.dll");
			validatePeLayout(image);
			Assert::IsTrue((get16(image, optionalHeader(image) + 70) & 0x40) != 0, L"DLL advertises ASLR");

			using Getter = uintptr_t(*)();
			vector<HMODULE> modules;
			for (const auto* name : { L"one.dll", L"two.dll" })
			{
				const auto module = LoadLibraryExW(workspace.file(name).c_str(), nullptr, 0);
				Assert::IsNotNull(module, (wstring(L"Windows loader accepts ") + name).c_str());
				modules.push_back(module);
				const auto get = reinterpret_cast<Getter>(GetProcAddress(module, "get"));
				const auto where = reinterpret_cast<Getter>(GetProcAddress(module, "where"));
				Assert::IsTrue(get && where, L"exports resolve through the loader");
				Assert::AreEqual<uintptr_t>(where(), get(), L"DIR64 relocation tracks the load address");
				const auto base = reinterpret_cast<uintptr_t>(module);
				Assert::IsTrue(where() >= base && where() < base + get32(image, optionalHeader(image) + 56), L"relocated pointer lies inside the loaded image");
			}
			Assert::IsTrue(modules[0] != modules[1], L"two images with one preferred base load at distinct addresses");
			for (const auto module : modules) FreeLibrary(module);
		}

		TEST_METHOD(DumpbinAcceptsLinkedImagesAndReportsExpectedStructure)
		{
			fs::path dumpbin;
			error_code ignored;
			wchar_t programFiles[MAX_PATH]{};
			GetEnvironmentVariableW(L"ProgramFiles", programFiles, MAX_PATH);
			if (programFiles)
			{
				for (const auto& entry : fs::recursive_directory_iterator(fs::path(programFiles) / L"Microsoft Visual Studio", fs::directory_options::skip_permission_denied, ignored))
				{
					if (entry.path().filename() == L"dumpbin.exe" && entry.path().parent_path().filename() == L"x64" && entry.path().parent_path().parent_path().filename() == L"Hostx64") dumpbin = entry.path();
				}
			}
			if (dumpbin.empty()) Logger::WriteMessage("dumpbin.exe not found; standard-tool inspection skipped");
			if (dumpbin.empty()) return;

			Workspace workspace;
			TestSection data{ ".data", Bytes(16, 0), 0xc0000040 };
			data.relocations = { { 0, 0, 1 } };
			TestSection text{ ".text",{ 0xb9, 42, 0, 0, 0, 0xe8, 0, 0, 0, 0, 0xcc } };
			text.relocations = { { 6, 2, 4 } };
			workspace.write(L"input.obj", makeObject({ data, text }, { TestSymbol{ "main", 0, 2, 2,{} }, TestSymbol{ "slot", 0, 1, 2,{} }, TestSymbol{ "ExitProcess", 0, 0, 2,{} } }));
			workspace.write(L"kernel.lib", makeArchive({ { "exit.obj", makeImportObject("ExitProcess", "KERNEL32.dll", "ExitProcess", 0, 4) } }));
			auto args = outputArgs(workspace);
			args.push_back(workspace.file(L"input.obj").wstring());
			args.push_back(workspace.file(L"kernel.lib").wstring());
			args.insert(args.begin() + 1, L"/BASE:0x140000000");
			assertSuccess(runLinker(workspace, args));
			workspace.write(L"dll.obj", makeObject({ TestSection{ ".text",{ 0xc3 } } }, { TestSymbol{ "api", 0, 1, 2,{} } }));
			assertSuccess(runLinker(workspace, { L"/DLL", L"/NOENTRY", L"/EXPORT:api", L"/OUT:" + workspace.file(L"tool.dll").wstring(), workspace.file(L"dll.obj").wstring() }));

			const auto exe = runExecutable(workspace, dumpbin, { L"/NOLOGO", L"/HEADERS", L"/IMPORTS", L"/RELOCATIONS", workspace.file(L"linked.exe").wstring() });
			Assert::AreEqual<DWORD>(0, exe.exitCode, wstring(exe.output.begin(), exe.output.end()).c_str());
			for (const auto* expected : { "PE signature found", "8664 machine (x64)", "PE32+", "Dynamic base", "KERNEL32.dll", "ExitProcess", "DIR64", "section alignment", ".text name" })
				Assert::IsTrue(exe.output.find(expected) != string::npos, (L"dumpbin output for the executable contains " + wstring(expected, expected + strlen(expected))).c_str());
			Assert::IsTrue(exe.output.find("warning") == string::npos && exe.output.find("invalid") == string::npos, L"dumpbin reports no PE warnings");

			const auto dll = runExecutable(workspace, dumpbin, { L"/NOLOGO", L"/HEADERS", L"/EXPORTS", workspace.file(L"tool.dll").wstring() });
			Assert::AreEqual<DWORD>(0, dll.exitCode, L"dumpbin accepts the DLL");
			for (const auto* expected : { "DLL", "api", "tool.dll" })
				Assert::IsTrue(dll.output.find(expected) != string::npos, (L"dumpbin output for the DLL contains " + wstring(expected, expected + strlen(expected))).c_str());
		}

		TEST_METHOD(JwlinkDirectiveSyntaxLinksLikeEquivalentSwitches)
		{
			Workspace workspace;
			TestSection text{ ".text",{ 0xb9, 42, 0, 0, 0, 0xe8, 0, 0, 0, 0, 0xcc } };
			text.relocations = { { 6, 1, 4 } };
			workspace.write(L"input.obj", makeObject({ text }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "ExitProcess", 0, 0, 2,{} } }));
			workspace.write(L"kernel.lib", makeArchive({ { "exit.obj", makeImportObject("ExitProcess", "KERNEL32.dll", "ExitProcess", 0, 4) } }));
			// Mirrors test\run.bat: format win pe, option map=..., file ..., lib ..., name ...
			assertSuccess(runLinker(workspace, { L"format", L"win", L"pe", L"option", L"map=directive.map,quiet,start=main", L"file", L"input", L"lib", L"kernel.lib", L"name", L"directive.exe" }));
			assertSuccess(runLinker(workspace, { L"/MAP:switch.map", L"/NOLOGO", L"/ENTRY:main", L"/OUT:switch.exe", L"input.obj", L"kernel.lib" }));
			Assert::IsTrue(workspace.read(L"directive.exe") == workspace.read(L"switch.exe"), L"directive and switch spellings produce identical images");
			Assert::IsTrue(fs::exists(workspace.file(L"directive.map")), L"option map=<file> writes the requested map");
			Assert::AreEqual<DWORD>(42, runExecutable(workspace, workspace.file(L"directive.exe"), {}).exitCode, L"directive-linked image executes");

			workspace.writeText(L"build.rsp", L"format windows pe\r\noption start=main\r\nfile input.obj\r\nlibrary kernel.lib\r\nname response.exe\r\n");
			assertSuccess(runLinker(workspace, { L"@build.rsp" }));
			Assert::IsTrue(workspace.read(L"response.exe") == workspace.read(L"switch.exe"), L"directives in a response file link identically");
		}

		TEST_METHOD(StackHeapMergeAndSectionControlsAreWrittenToTheImage)
		{
			Workspace workspace;
			TestSection rdata{ ".rdata",{ 1, 2, 3, 4 }, 0x40000040 };
			TestSection text{ ".text",{ 0xc3 } };
			TestSection custom{ ".custom",{ 9 }, 0x40000040 };
			workspace.write(L"input.obj", makeObject({ text, rdata, custom }, { TestSymbol{ "main", 0, 1, 2,{} } }));
			auto args = outputArgs(workspace);
			for (const auto* option : { L"/STACK:0x400000,0x8000", L"/HEAP:0x200000,0x4000", L"/MERGE:.rdata=.custom", L"/SECTION:.custom,RW" }) args.insert(args.begin() + 1, option);
			args.push_back(workspace.file(L"input.obj").wstring());
			assertSuccess(runLinker(workspace, args));
			const auto image = workspace.read(L"linked.exe");
			const auto optional = optionalHeader(image);
			Assert::AreEqual<uint64_t>(0x400000, get64(image, optional + 72));
			Assert::AreEqual<uint64_t>(0x8000, get64(image, optional + 80));
			Assert::AreEqual<uint64_t>(0x200000, get64(image, optional + 88));
			Assert::AreEqual<uint64_t>(0x4000, get64(image, optional + 96));
			const auto custom0 = sectionHeader(image, ".custom");
			Assert::AreEqual<uint32_t>(0xc0000000u, get32(image, custom0 + 36) & 0xfe000000u);
			const auto raw = get32(image, custom0 + 20);
			Assert::AreEqual<uint8_t>(1, image[raw], L".rdata bytes lead the merged section");
			Assert::IsTrue(find(image.begin() + raw, image.begin() + raw + get32(image, custom0 + 16), uint8_t{ 9 }) != image.begin() + raw + get32(image, custom0 + 16), L".custom bytes follow");
			const auto sectionCount = get16(image, peHeader(image) + 6);
			for (size_t i = 0; i < sectionCount; ++i)
			{
				const auto name = reinterpret_cast<const char*>(image.data() + sectionTable(image) + i * 40);
				Assert::IsTrue(strncmp(name, ".rdata", 8) != 0, L".rdata was merged away");
			}
			validatePeLayout(image);
		}

		TEST_METHOD(WholeArchiveAndIncludePullInUnreferencedMembers)
		{
			Workspace workspace;
			workspace.write(L"input.obj", makeEntryObject());
			const auto extra = makeObject({ TestSection{ ".text",{ 0xaa, 0xbb, 0xcc, 0xdd, 0xee } } }, { TestSymbol{ "unused_fn", 0, 1, 2,{} } });
			workspace.write(L"x.lib", makeArchive({ { "extra.obj", extra } }));
			const auto contains = [](const Bytes& image) { const Bytes needle{ 0xaa, 0xbb, 0xcc, 0xdd, 0xee }; return search(image.begin(), image.end(), needle.begin(), needle.end()) != image.end(); };
			auto link = [&](const wstring& out, vector<wstring> extraArgs)
			{
				vector<wstring> args{ L"/ENTRY:main", L"/OUT:" + workspace.file(out).wstring() };
				args.insert(args.end(), extraArgs.begin(), extraArgs.end());
				args.push_back(workspace.file(L"input.obj").wstring());
				args.push_back(workspace.file(L"x.lib").wstring());
				assertSuccess(runLinker(workspace, args));
				return workspace.read(out);
			};
			Assert::IsFalse(contains(link(L"plain.exe", {})), L"unreferenced member stays out by default");
			Assert::IsTrue(contains(link(L"whole.exe", { L"/WHOLEARCHIVE:x.lib" })), L"/WHOLEARCHIVE pulls in every member");
			Assert::IsTrue(contains(link(L"all.exe", { L"/WHOLEARCHIVE" })), L"bare /WHOLEARCHIVE applies to all archives");
			Assert::IsTrue(contains(link(L"inc.exe", { L"/INCLUDE:unused_fn" })), L"/INCLUDE forces the defining member");
			const auto missing = runLinker(workspace, { L"/ENTRY:main", L"/INCLUDE:nope", L"/OUT:" + workspace.file(L"m.exe").wstring(), workspace.file(L"input.obj").wstring() });
			assertFailureContains(missing, "nope");
		}

		TEST_METHOD(VerboseReportsInputsAndLayout)
		{
			Workspace workspace;
			workspace.write(L"input.obj", makeEntryObject());
			auto args = outputArgs(workspace);
			args.insert(args.begin() + 1, L"/VERBOSE");
			args.push_back(workspace.file(L"input.obj").wstring());
			const auto result = runLinker(workspace, args);
			assertSuccess(result);
			Assert::IsTrue(result.output.find("jwlink2: object") != string::npos && result.output.find("jwlink2: section .text") != string::npos && result.output.find("jwlink2: wrote") != string::npos);
		}

		TEST_METHOD(ManifestIsEmbeddedAsResourceOrWrittenExternally)
		{
			Workspace workspace;
			workspace.write(L"input.obj", makeEntryObject());
			const auto input = workspace.file(L"input.obj").wstring();
			assertSuccess(runLinker(workspace, { L"/ENTRY:main", L"/MANIFEST:EMBED", L"/OUT:" + workspace.file(L"embed.exe").wstring(), input }));
			const auto image = workspace.read(L"embed.exe");
			validatePeLayout(image);
			const auto resource = sectionHeader(image, ".rsrc");
			const auto base = get32(image, resource + 20);
			const auto directoryRva = get32(image, optionalHeader(image) + 112 + 16);
			Assert::AreEqual(get32(image, resource + 12), directoryRva, L"resource data directory points at .rsrc");
			Assert::AreEqual<uint32_t>(24, get32(image, base + 16));
			Assert::AreEqual<uint32_t>(1, get32(image, base + 24 + 16), L"EXE manifest resource ID is 1");
			const auto dataRva = get32(image, base + 72);
			const auto size = get32(image, base + 76);
			const string text(reinterpret_cast<const char*>(image.data() + base + (dataRva - directoryRva)), size);
			Assert::IsTrue(text.find("requestedExecutionLevel") != string::npos);

			workspace.writeText(L"app.manifest", L"<assembly>custom</assembly>");
			assertSuccess(runLinker(workspace, { L"/DLL", L"/NOENTRY", L"/EXPORT:main", L"/MANIFESTINPUT:" + workspace.file(L"app.manifest").wstring(), L"/OUT:" + workspace.file(L"m.dll").wstring(), input }));
			const auto dll = workspace.read(L"m.dll");
			const auto dllBase = get32(dll, sectionHeader(dll, ".rsrc") + 20);
			Assert::AreEqual<uint32_t>(2, get32(dll, dllBase + 24 + 16), L"DLL manifest resource ID is 2");
			const string custom(reinterpret_cast<const char*>(dll.data() + dllBase + 88), get32(dll, dllBase + 76));
			Assert::AreEqual<string>("<assembly>custom</assembly>", custom);

			assertSuccess(runLinker(workspace, { L"/ENTRY:main", L"/MANIFEST", L"/OUT:" + workspace.file(L"ext.exe").wstring(), input }));
			Assert::IsTrue(fs::exists(workspace.file(L"ext.exe.manifest")));
			Assert::IsTrue(workspace.read(L"ext.exe").size() > 0);
			Assert::IsTrue(fs::exists(workspace.file(L"ext.exe.manifest")) && !fs::exists(workspace.file(L"plain.manifest")));

			TestSection rsrc{ ".rsrc",{ 0, 0, 0, 0 }, 0x40000040 };
			workspace.write(L"res.obj", makeObject({ TestSection{ ".text",{ 0xc3 } }, rsrc }, { TestSymbol{ "main", 0, 1, 2,{} } }));
			assertFailureContains(runLinker(workspace, { L"/ENTRY:main", L"/MANIFEST:EMBED", L"/OUT:" + workspace.file(L"conflict.exe").wstring(), workspace.file(L"res.obj").wstring() }), "already contains a .rsrc");
		}

		TEST_METHOD(OptionMatrixAcceptsAndRejectsDocumentedForms)
		{
			Workspace workspace;
			workspace.write(L"input.obj", makeEntryObject());
			workspace.write(L"dll.obj", makeObject({ TestSection{ ".text",{ 0xc3 } } }, { TestSymbol{ "api", 0, 1, 2,{} } }));
			workspace.write(L"kernel.lib", makeArchive({ { "exit.obj", makeImportObject("ExitProcess", "KERNEL32.dll", "ExitProcess", 0, 4) } }));
			struct Row { vector<wstring> args; const char* rejected; };
			const vector<Row> matrix = {
				// JWlink directives: accepted
				{ { L"format", L"windows", L"pe", L"file", L"input.obj", L"name", L"m1.exe", L"option", L"start=main" }, nullptr },
				{ { L"form", L"win", L"nt", L"fi", L"input", L"na", L"m2", L"op", L"start=main,quiet,caseexact" }, nullptr },
				{ { L"format", L"windows", L"pe", L"dll", L"file", L"dll.obj", L"export", L"api", L"option", L"implib=m3.lib,nodefaultlibs", L"name", L"m3.dll" }, nullptr },
				{ { L"system", L"nt_dll", L"file", L"dll.obj", L"export", L"api", L"name", L"m4.dll" }, nullptr },
				{ { L"system", L"nt_win", L"file", L"input.obj", L"option", L"start=main" }, nullptr },
				{ { L"runtime", L"console=6.0", L"debug", L"all", L"file", L"input.obj", L"option", L"start=main,map" }, nullptr },
				{ { L"format", L"win", L"pe", L"file", L"input.obj", L"libpath", L".", L"library", L"kernel", L"disable", L"1014", L"option", L"start=main,offset=0x10000,objalign=8192,norelocs" }, nullptr },
				// JWlink directives: rejected explicitly
				{ { L"format", L"os2", L"file", L"input.obj" }, "unsupported FORMAT" },
				{ { L"format", L"windows", L"file", L"input.obj" }, "requires PE" },
				{ { L"format", L"windows", L"pe", L"elf", L"file", L"input.obj" }, "unsupported FORMAT modifier" },
				{ { L"system", L"dos", L"file", L"input.obj" }, "unsupported SYSTEM" },
				{ { L"runtime", L"native", L"file", L"input.obj" }, "unsupported RUNTIME" },
				{ { L"debug", L"dwarf", L"file", L"input.obj" }, "unsupported DEBUG format" },
				{ { L"option", L"stack=bad", L"file", L"input.obj" }, "JWlink size option" },
				{ { L"option", L"linkversion=256.0", L"file", L"input.obj" }, "invalid JWlink version" },
				{ { L"option", L"nxcompat=no", L"file", L"input.obj" }, "does not accept" },
				{ { L"option", L"eliminate", L"file", L"input.obj" }, "unsupported JWlink OPTION" },
				{ { L"option", L"start", L"file", L"input.obj" }, "requires a value" },
				{ { L"alias", L"a=b", L"file", L"input.obj" }, "unsupported JWlink directive 'alias'" },
				{ { L"segment", L"x", L"file", L"input.obj" }, "unsupported JWlink directive 'segment'" },
				{ { L"order", L"x", L"file", L"input.obj" }, "unsupported JWlink directive 'order'" },
				{ { L"format", L"win", L"pe", L"file" }, "requires an argument" },
				{ { L"format", L"win", L"pe", L"name", L"a.exe", L"b.exe", L"file", L"input.obj" }, "exactly one output" },
				{ { L"format", L"win", L"pe", L"export", L"a.bad", L"file", L"input.obj" }, "ordinal" },
				{ { L"format", L"win", L"pe", L"anonymousexport", L"a", L"file", L"input.obj" }, "NONAME requires" },
				{ { L"format", L"win", L"pe", L"disable", L"abc", L"file", L"input.obj" }, "DISABLE requires" },
				{ { L"format", L"win", L"pe", L"/OUT:x.exe", L"file", L"input.obj" }, "cannot be mixed" },
				// MSVC-style switches: accepted
				{ { L"/ENTRY:main", L"/OUT:s1.exe", L"/NOLOGO", L"/MACHINE:X64", L"/INCREMENTAL:NO", L"input.obj" }, nullptr },
				{ { L"/ENTRY:main", L"/OUT:s2.exe", L"/NXCOMPAT", L"/DYNAMICBASE:NO", L"/SUBSYSTEM:WINDOWS,6.0", L"input.obj" }, nullptr },
				// Switches whose behavior JWlink2 does not implement are rejected, never silently ignored.
				// Controls that are implemented are accepted; invalid arguments are diagnosed.
				{ { L"/ENTRY:main", L"/OUT:s3.exe", L"/MANIFEST:EMBED", L"/STACK:0x200000,0x2000", L"/HEAP:0x200000", L"/VERBOSE", L"input.obj" }, nullptr },
				{ { L"/ENTRY:main", L"/OUT:s4.exe", L"/MANIFEST", L"/MERGE:.rdata=.text", L"/SECTION:.text,ER", L"/INCLUDE:main", L"/WHOLEARCHIVE:kernel.lib", L"/WHOLEARCHIVE", L"/MANIFEST:NO", L"input.obj", L"kernel.lib" }, nullptr },
				{ { L"/MANIFEST:BOGUS", L"input.obj" }, "/MANIFEST accepts" },
				{ { L"/STACK:0x1000,0x2000", L"input.obj" }, "reserve must be nonzero" },
				{ { L"/HEAP:abc", L"input.obj" }, "/HEAP" },
				{ { L"/MERGE:.rdata", L"input.obj" }, "/MERGE requires" },
				{ { L"/MERGE:.pdata=.text", L"input.obj" }, "linker-managed" },
				{ { L"/MERGE:.a=.a", L"input.obj" }, "same section" },
				{ { L"/MERGE:.a=.b", L"/MERGE:.b=.c", L"input.obj" }, "chain" },
				{ { L"/SECTION:.x,Q", L"input.obj" }, "is not one of" },
				{ { L"/SECTION:.x", L"input.obj" }, "/SECTION requires" },
				{ { L"/VERBOSE:1", L"input.obj" }, "does not accept a value" },
				{ { L"/INCLUDE:", L"input.obj" }, "requires a symbol" },
				{ { L"/MANIFESTINPUT:missing.manifest", L"input.obj" }, "missing.manifest" },
				{ { L"/OPT:REF", L"input.obj" }, "unsupported option" },
				{ { L"/LTCG", L"input.obj" }, "unsupported option" },
				{ { L"/MACHINE:ARM64", L"input.obj" }, "only supports /MACHINE:X64" },
				{ { L"/INCREMENTAL", L"input.obj" }, "INCREMENTAL:NO" }
			};
			for (const auto& row : matrix)
			{
				const auto result = runLinker(workspace, row.args);
				wstring label;
				for (const auto& argument : row.args) label += argument + L" ";
				if (row.rejected) Assert::IsTrue(result.exitCode != 0 && result.output.find(row.rejected) != string::npos, (L"expected rejection '" + wstring(row.rejected, row.rejected + strlen(row.rejected)) + L"' for: " + label + wstring(result.output.begin(), result.output.end())).c_str());
				else Assert::AreEqual<DWORD>(0, result.exitCode, (L"expected success for: " + label + wstring(result.output.begin(), result.output.end())).c_str());
			}
			Assert::IsTrue(fs::exists(workspace.file(L"m2.exe")), L"directive name without an extension gets .exe");
			Assert::IsTrue(fs::exists(workspace.file(L"m3.dll")) && fs::exists(workspace.file(L"m3.lib")), L"directive DLL link writes the DLL and import library");
		}

		TEST_METHOD(JwlinkPeOptionsMatchHeaderFields)
		{
			Workspace original;
			Workspace replacement;
			const auto object = makeObject({ TestSection{ ".text", { 0xc3 } } }, { TestSymbol{ "main", 0, 1, 2,{} } });
			original.write(L"program.obj", object);
			replacement.write(L"program.obj", object);
			const vector<wstring> args{ L"format", L"win", L"pe", L"option", L"quiet,start=main,stack=2048,heapsize=3072,version=7.8,osversion=10.4,linkversion=12.34,nxcompat,nolargeaddressaware", L"file", L"program.obj", L"name", L"out.exe" };
			assertSuccess(runExecutable(original, jwlinkPath(), args));
			assertSuccess(runLinker(replacement, args));
			const auto expected = original.read(L"out.exe");
			const auto actual = replacement.read(L"out.exe");
			const auto expectedOptional = optionalHeader(expected);
			const auto actualOptional = optionalHeader(actual);
			for (const size_t offset : { 2u, 3u }) Assert::AreEqual<uint8_t>(expected[expectedOptional + offset], actual[actualOptional + offset]);
			for (const size_t offset : { 40u, 42u, 44u, 46u }) Assert::AreEqual<uint16_t>(get16(expected, expectedOptional + offset), get16(actual, actualOptional + offset));
			for (const size_t offset : { 72u, 80u, 88u, 96u }) Assert::AreEqual<uint64_t>(get64(expected, expectedOptional + offset), get64(actual, actualOptional + offset));
			Assert::AreEqual<uint16_t>(peFileCharacteristics(expected) & 0x20, peFileCharacteristics(actual) & 0x20);
			Assert::AreEqual<uint16_t>(get16(expected, expectedOptional + 70) & 0x100, get16(actual, actualOptional + 70) & 0x100);
			const vector<wstring> repeated{ L"format", L"win", L"pe", L"option", L"quiet,start=main,stack=8192,stack=512,heapsize=1024,heapsize=8192,version=2.3,version=4,osversion=6.7,osversion=8,linkversion=4.5,linkversion=9,nolargeaddressaware,largeaddressaware", L"file", L"program.obj", L"name", L"repeat.exe" };
			assertSuccess(runExecutable(original, jwlinkPath(), repeated));
			assertSuccess(runLinker(replacement, repeated));
			const auto originalRepeat = original.read(L"repeat.exe");
			const auto replacementRepeat = replacement.read(L"repeat.exe");
			const auto eo = optionalHeader(originalRepeat), ao = optionalHeader(replacementRepeat);
			for (const size_t offset : { 2u, 3u }) Assert::AreEqual<uint8_t>(originalRepeat[eo + offset], replacementRepeat[ao + offset]);
			for (const size_t offset : { 40u, 42u, 44u, 46u }) Assert::AreEqual<uint16_t>(get16(originalRepeat, eo + offset), get16(replacementRepeat, ao + offset));
			for (const size_t offset : { 72u, 80u, 88u, 96u }) Assert::AreEqual<uint64_t>(get64(originalRepeat, eo + offset), get64(replacementRepeat, ao + offset));
			Assert::AreEqual<uint16_t>(peFileCharacteristics(originalRepeat) & 0x20, peFileCharacteristics(replacementRepeat) & 0x20);
		}

		TEST_METHOD(JwlinkExportOrdinalAliasAndAnonymousWorkAtRuntime)
		{
			Workspace workspace;
			workspace.write(L"input.obj", makeObject({ TestSection{ ".text", { 0xb8, 42, 0, 0, 0, 0xc3 } } }, { TestSymbol{ "func", 0, 1, 2,{} } }));
			assertSuccess(runLinker(workspace, { L"format", L"win", L"pe", L"dll", L"file", L"input.obj", L"export", L"publicName.5=func", L"anonymousexport", L"hidden.7=func", L"name", L"out.dll" }));
			const auto module = LoadLibraryW(workspace.file(L"out.dll").c_str());
			Assert::IsNotNull(module, L"Windows loads the linked DLL");
			const auto named = GetProcAddress(module, "publicName");
			const auto ordinal = GetProcAddress(module, MAKEINTRESOURCEA(5));
			const auto anonymous = GetProcAddress(module, MAKEINTRESOURCEA(7));
			Assert::IsNotNull(named);
			Assert::IsTrue(named == ordinal && ordinal == anonymous, L"ordinals and alias target the same function");
			Assert::IsNull(GetProcAddress(module, "hidden"), L"anonymous export has no name");
			Assert::AreEqual<int>(42, reinterpret_cast<int(*)()>(named)());
			FreeLibrary(module);
		}

		TEST_METHOD(JwlinkReferencePullsArchiveMember)
		{
			Workspace workspace;
			workspace.write(L"input.obj", makeEntryObject());
			workspace.write(L"helpers.lib", makeArchive({ { "helper.obj", makeObject({ TestSection{ ".text", { 0x42, 0x53, 0x64 } } }, { TestSymbol{ "helper", 0, 1, 2,{} } }) } }));
			const auto result = runLinker(workspace, { L"format", L"win", L"pe", L"option", L"start=main", L"file", L"input.obj", L"library", L"helpers.lib", L"reference", L"helper", L"name", L"out.exe" });
			assertSuccess(result);
			const auto image = workspace.read(L"out.exe");
			const auto header = sectionHeader(image, ".text");
			const auto raw = get32(image, header + 20);
			const Bytes needle{ 0x42, 0x53, 0x64 };
			Assert::IsTrue(search(image.begin() + raw, image.begin() + raw + get32(image, header + 16), needle.begin(), needle.end()) != image.begin() + raw + get32(image, header + 16));
			assertFailureContains(runLinker(workspace, { L"format", L"win", L"pe", L"file", L"input.obj", L"reference", L"missing", L"name", L"fail.exe" }), "missing");
		}

		TEST_METHOD(JwlinkDirectiveDefaultsAndExplicitEntry)
		{
			Workspace original;
			Workspace replacement;
			const auto object = makeObject({ TestSection{ ".text", { 0x90, 0xc3 } } }, { TestSymbol{ "main", 1, 1, 2,{} } });
			original.write(L"program.obj", object);
			replacement.write(L"program.obj", object);
			const vector<wstring> args{ L"format", L"win", L"pe", L"option", L"start=main", L"file", L"program.obj" };
			assertSuccess(runExecutable(original, jwlinkPath(), args));
			assertSuccess(runLinker(replacement, args));
			const auto expected = original.read(L"program.exe");
			const auto actual = replacement.read(L"program.exe");
			for (const auto* image : { &expected, &actual })
			{
				Assert::AreEqual<uint16_t>(3, get16(*image, optionalHeader(*image) + 68));
				const auto entry = get32(*image, optionalHeader(*image) + 16);
				Assert::AreEqual<uint8_t>(0xc3, (*image)[rvaOffset(*image, entry)], L"explicit start selects main");
			}
			const auto defaultResult = runLinker(replacement, { L"format", L"win", L"pe", L"file", L"program.obj", L"name", L"fallback.exe" });
			assertSuccess(defaultResult);
			Assert::IsTrue(defaultResult.output.find("warning: no start address") != string::npos);
			const auto fallback = replacement.read(L"fallback.exe");
			Assert::AreEqual<uint8_t>(0x90, fallback[rvaOffset(fallback, get32(fallback, optionalHeader(fallback) + 16))], L"directive-mode fallback starts at the first section");
			const vector<wstring> guiArgs{ L"format", L"win", L"pe", L"runtime", L"windows", L"option", L"start=main", L"file", L"program.obj", L"name", L"gui.exe" };
			assertSuccess(runExecutable(original, jwlinkPath(), guiArgs));
			assertSuccess(runLinker(replacement, guiArgs));
			const auto originalGui = original.read(L"gui.exe");
			const auto replacementGui = replacement.read(L"gui.exe");
			Assert::AreEqual<uint16_t>(2, get16(originalGui, optionalHeader(originalGui) + 68));
			Assert::AreEqual<uint16_t>(2, get16(replacementGui, optionalHeader(replacementGui) + 68));
			assertSuccess(runLinker(replacement, { L"/OUT:switch.exe", L"program.obj" }));
			const auto switched = replacement.read(L"switch.exe");
			Assert::AreEqual<uint8_t>(0xc3, switched[rvaOffset(switched, get32(switched, optionalHeader(switched) + 16))], L"switch-mode default remains main");
		}

		TEST_METHOD(RepresentativeObjectMatchesJWlinkPeBehavior)
		{
			Workspace workspace;
			const auto object = makeObject(
				{ TestSection{ ".text",{ 0xc3, 0x2a } }, makeDirectiveSection("-entry:main ") },
				{ TestSymbol{ "main", 0, 1, 2,{} } });
			workspace.write(L"input.obj", object);
			const auto jwlink2Args = vector<wstring>{ L"/OUT:" + workspace.file(L"jwlink2.exe").wstring(), workspace.file(L"input.obj").wstring() };
			assertSuccess(runExecutable(workspace, jwlink2Path(), jwlink2Args));
			const auto jwlinkArgs = vector<wstring>{
				L"format", L"win", L"pe", L"option", L"quiet,start=main",
				L"file", workspace.file(L"input.obj").wstring(),
				L"name", workspace.file(L"jwlink.exe").wstring()
			};
			assertSuccess(runExecutable(workspace, jwlinkPath(), jwlinkArgs));

			const auto jwlink2Image = workspace.read(L"jwlink2.exe");
			const auto jwlinkImage = workspace.read(L"jwlink.exe");
			for (const auto* image : { &jwlink2Image, &jwlinkImage })
			{
				const auto optional = optionalHeader(*image);
				const auto pe = peHeader(*image);
				Assert::AreEqual<uint16_t>(0x8664, get16(*image, pe + 4), L"both linkers target AMD64");
				Assert::IsTrue((peFileCharacteristics(*image) & 0x2000) == 0, L"both outputs are executables, not DLLs");
				Assert::AreEqual<uint16_t>(3, get16(*image, optional + 68), L"both outputs use the console subsystem");
				const auto entryRva = get32(*image, optional + 16);
				const auto entryOffset = rvaOffset(*image, entryRva);
				Assert::AreEqual<uint8_t>(0xc3, (*image)[entryOffset], L"both entry points execute the return instruction");
				const auto text = sectionHeader(*image, ".text");
				Assert::IsTrue(get32(*image, text + 16) >= 2, L"both images preserve initialized code bytes");
				const auto textOffset = get32(*image, text + 20);
				Assert::AreEqual<uint8_t>(0xc3, (*image)[textOffset], L"both images preserve the entry-point instruction");
				Assert::AreEqual<uint8_t>(0x2a, (*image)[textOffset + 1], L"both images preserve the second code-section byte");
			}
		}

		TEST_METHOD(OverflowRelocationsAtBoundaryAndInArchivesAreLinked)
		{
			for (const uint32_t count : { 65534u, 65535u, 65536u })
			{
				Workspace workspace;
				TestSection data{ ".data", Bytes(static_cast<size_t>(count) * 4, 0), 0xc0000040 };
				for (uint32_t i = 0; i < count; ++i) data.relocations.push_back({ i * 4, 0, 3 });
				const auto member = count == 65536 ? makeBigObjObject({ data }, { TestSymbol{ "target", 0, 1, 2,{} } }) : makeObject({ data }, { TestSymbol{ "target", 0, 1, 2,{} } });
				TestSection text{ ".text", Bytes(4, 0) };
				text.relocations = { { 0, 1, 3 } };
				workspace.write(L"input.obj", makeObject({ text }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "target", 0, 0, 2,{} } }));
				workspace.write(L"members.lib", makeArchive({ { "member.obj", member } }));
				assertSuccess(runLinker(workspace, { L"/ENTRY:main", L"/OUT:linked.exe", L"input.obj", L"members.lib" }));
				const auto image = workspace.read(L"linked.exe");
				const auto header = sectionHeader(image, ".data");
				const auto raw = get32(image, header + 20);
				const auto rva = get32(image, header + 12);
				Assert::AreEqual<uint32_t>(rva, get32(image, raw), L"first relocation is applied");
				Assert::AreEqual<uint32_t>(rva, get32(image, raw + (count - 1) * 4), L"last relocation is applied");
				Assert::AreEqual<uint32_t>(0, get32(image, header + 36) & 0x01000000u, L"COFF overflow flag is not an image section flag");
			}
		}

		TEST_METHOD(OrdinaryMaximumCountAndAbsoluteNoOpAreAccepted)
		{
			Workspace workspace;
			TestSection data{ ".data", Bytes(4, 0), 0xc0000040 };
			data.relocations.resize(65535, TestRelocation{ 0xffffffffu, 0xffffffffu, 0 });
			auto object = makeObject({ data, TestSection{ ".text", { 0xc3 } } }, { TestSymbol{ "main", 0, 2, 2,{} } });
			const auto record = get32(object, 20 + 24);
			object.erase(object.begin() + record, object.begin() + record + 10);
			put32(object, 8, get32(object, 8) - 10);
			put32(object, 20 + 36, 0xc0000040);
			assertSuccess(linkObject(workspace, object));
		}

		TEST_METHOD(MalformedOverflowRelocationTablesAreRejected)
		{
			TestSection data{ ".data", Bytes(4, 0), 0xc0000040 };
			data.relocations.resize(65535, TestRelocation{ 0, 0, 0 });
			const auto original = makeObject({ data, TestSection{ ".text", { 0xc3 } } }, { TestSymbol{ "main", 0, 2, 2,{} } });
			const auto record = get32(original, 20 + 24);
			for (int variant = 0; variant < 7; ++variant)
			{
				Workspace workspace;
				auto object = original;
				if (variant == 0) put16(object, 20 + 32, 1);
				if (variant == 1) put32(object, 20 + 24, 0);
				if (variant == 2) put32(object, record, 65535);
				if (variant == 3) put32(object, record + 4, 1);
				if (variant == 4) put16(object, record + 8, 1);
				if (variant == 5) put32(object, record, 0xffffffffu);
				if (variant == 6) put32(object, 20 + 24, static_cast<uint32_t>(object.size() - 5));
				const auto result = linkObject(workspace, object);
				assertFailureContains(result, variant == 5 ? "relocation table" : "relocation overflow");
				assertNoOutput(workspace);
			}
		}

		TEST_METHOD(RealMsvcAndMasmRelocationsAreLinked)
		{
			fs::path toolDirectory;
			error_code ignored;
			wchar_t programFiles[MAX_PATH]{};
			GetEnvironmentVariableW(L"ProgramFiles", programFiles, MAX_PATH);
			for (const auto& entry : fs::recursive_directory_iterator(fs::path(programFiles) / L"Microsoft Visual Studio", fs::directory_options::skip_permission_denied, ignored))
			{
				if (entry.path().filename() == L"ml64.exe" && entry.path().parent_path().filename() == L"x64" && entry.path().parent_path().parent_path().filename() == L"Hostx64" && fs::exists(entry.path().parent_path() / L"cl.exe")) toolDirectory = entry.path().parent_path();
			}
			if (toolDirectory.empty()) { Logger::WriteMessage("MSVC x64 tools not found; real-object relocation audit skipped"); return; }
			Workspace workspace;
			workspace.writeText(L"overflow.asm", L"option casemap:none\n.data\npublic target\ntarget dq 0\npublic table\ntable label byte\nrept 65535\n dq target\nendm\n.code\npublic main\nmain proc\n mov eax, 42\n ret\nmain endp\nend\n");
			assertSuccess(runExecutable(workspace, toolDirectory / L"ml64.exe", { L"/nologo", L"/c", L"/Fooverflow.obj", L"overflow.asm" }));
			const auto object = workspace.read(L"overflow.obj");
			bool foundOverflow = false;
			for (uint16_t i = 0; i < get16(object, 2); ++i)
			{
				const auto header = 20 + static_cast<size_t>(i) * 40;
				if ((get32(object, header + 36) & 0x01000000u) == 0) continue;
				foundOverflow = true;
				Assert::AreEqual<uint16_t>(0xffff, get16(object, header + 32));
				Assert::AreEqual<uint32_t>(65536, get32(object, get32(object, header + 24)));
			}
			Assert::IsTrue(foundOverflow, L"MASM emitted an overflow table");
			assertSuccess(runLinker(workspace, { L"/ENTRY:main", L"/OUT:overflow.exe", L"overflow.obj" }));
			const auto image = workspace.read(L"overflow.exe");
			const auto data = sectionHeader(image, ".data");
			const auto raw = get32(image, data + 20);
			const auto dataRva = get32(image, data + 12);
			const auto target = get64(image, optionalHeader(image) + 24) + dataRva;
			Assert::AreEqual<uint64_t>(target, get64(image, raw + 8));
			Assert::AreEqual<uint64_t>(target, get64(image, raw + 8 + 65534 * 8));
			assertHasBaseReloc(image, dataRva + 8 + 65534 * 8, 10);
			Assert::AreEqual<DWORD>(42, runExecutable(workspace, workspace.file(L"overflow.exe"), {}).exitCode);

			workspace.writeText(L"tls.c", L"__declspec(thread) int values[2] = { 7, 9 };\nint main(void) { return values[1]; }\n");
			assertSuccess(runExecutable(workspace, toolDirectory / L"cl.exe", { L"/nologo", L"/c", L"/Z7", L"/O1", L"/GS-", L"/Fotls.obj", L"tls.c" }));
			const auto tlsObject = workspace.read(L"tls.obj");
			uint32_t secRelOffset = 0, expectedOffset = 0;
			bool foundSecRel = false;
			for (uint16_t i = 0; i < get16(tlsObject, 2); ++i)
			{
				const auto header = 20 + static_cast<size_t>(i) * 40;
				const string name(reinterpret_cast<const char*>(tlsObject.data() + header), 8);
				if (name.rfind(".text", 0) != 0) continue;
				const auto relocs = get32(tlsObject, header + 24);
				for (uint16_t r = 0; r < get16(tlsObject, header + 32); ++r)
				{
					const auto entry = relocs + static_cast<size_t>(r) * 10;
					if (get16(tlsObject, entry + 8) != 11) continue;
					foundSecRel = true;
					secRelOffset = get32(tlsObject, entry);
					expectedOffset = get32(tlsObject, get32(tlsObject, 8) + static_cast<size_t>(get32(tlsObject, entry + 4)) * 18 + 8) + get32(tlsObject, get32(tlsObject, header + 20) + secRelOffset);
				}
			}
			Assert::IsTrue(foundSecRel, L"MSVC emitted runtime SECREL for TLS");
			workspace.write(L"index.obj", makeObject({ TestSection{ ".data", Bytes(4, 0), 0xc0000040 }, TestSection{ ".tls", Bytes(16, 0), 0xc0000040 } }, { TestSymbol{ "_tls_index", 0, 1, 2,{} } }));
			assertSuccess(runLinker(workspace, { L"/NODEFAULTLIB", L"/ENTRY:main", L"/OUT:tls.exe", L"tls.obj", L"index.obj" }));
			const auto tlsImage = workspace.read(L"tls.exe");
			Assert::AreEqual<uint32_t>(expectedOffset + 16, get32(tlsImage, get32(tlsImage, sectionHeader(tlsImage, ".text") + 20) + secRelOffset), L"MSVC TLS reference includes the preceding contribution");
		}

		TEST_METHOD(RuntimeSectionAndSecrelUseFinalOutputSection)
		{
			Workspace workspace;
			TestSection references{ ".data", Bytes(10, 0), 0xc0000040 };
			put32(references.data, 2, 3);
			put32(references.data, 6, 0xffffffffu);
			references.relocations = { { 0, 1, 10 }, { 2, 1, 11 }, { 6, 1, 11 } };
			workspace.write(L"input.obj", makeObject({ TestSection{ ".text", { 0xc3 } }, references, TestSection{ ".target$Z", { 0x44 }, 0x40000040 } },
				{ TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "target", 0, 0, 2,{} } }));
			workspace.write(L"target.obj", makeObject({ TestSection{ ".target$A", { 0, 0x11, 0x22 }, 0x40000040 } }, { TestSymbol{ "target", 1, 1, 2,{} } }));
			assertSuccess(runLinker(workspace, { L"/ENTRY:main", L"/OUT:linked.exe", L"input.obj", L"target.obj" }));
			const auto image = workspace.read(L"linked.exe");
			const auto targetHeader = sectionHeader(image, ".target");
			const auto dataHeader = sectionHeader(image, ".data");
			const auto dataRaw = get32(image, dataHeader + 20);
			const auto expectedSection = static_cast<uint16_t>((targetHeader - sectionTable(image)) / 40 + 1);
			Assert::AreEqual<uint16_t>(expectedSection, get16(image, dataRaw), L"SECTION is the output PE section number");
			Assert::AreEqual<uint32_t>(4, get32(image, dataRaw + 2), L"SECREL includes sorted contribution offset, symbol value, and addend");
			Assert::AreEqual<uint32_t>(0, get32(image, dataRaw + 6), L"SECREL supports a negative encoded addend");
		}

		TEST_METHOD(SectionRelativeRelocationsValidateTargetsAndWidths)
		{
			for (int variant = 0; variant < 4; ++variant)
			{
				Workspace workspace;
				TestSection data{ ".data", Bytes(variant == 0 ? 1 : variant == 1 ? 3 : 4, 0), 0xc0000040 };
				data.relocations = { { 0, 1, static_cast<uint16_t>(variant == 0 || variant == 3 ? 10 : 11) } };
				const auto object = makeObject({ TestSection{ ".text", { 0xc3 } }, data }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "absolute", 7, -1, 3,{} } });
				const auto result = linkObject(workspace, object);
				if (variant < 2) assertFailureContains(result, "outside initialized section data");
				else if (variant == 2) assertFailureContains(result, "target has no output section");
				else
				{
					assertSuccess(result);
					const auto image = workspace.read(L"linked.exe");
					Assert::AreEqual<uint16_t>(static_cast<uint16_t>(get16(image, peHeader(image) + 6) + 1), get16(image, get32(image, sectionHeader(image, ".data") + 20)));
				}
			}
		}

		TEST_METHOD(Rel32ThroughRel32FiveAreAppliedWithCorrectPlaceBias)
		{
			Workspace workspace;
			TestSection text{ ".text", Bytes(24, 0) };
			for (uint16_t type = 4; type <= 9; ++type) text.relocations.push_back({ static_cast<uint32_t>((type - 4) * 4), 0, type });
			const auto object = makeObject({ text }, { TestSymbol{ "main", 0, 1, 2,{} } });
			assertSuccess(linkObject(workspace, object));
			const auto image = workspace.read(L"linked.exe");
			const auto section = sectionHeader(image, ".text");
			const auto raw = get32(image, section + 20);
			const auto sectionRva = get32(image, section + 12);
			for (uint16_t type = 4; type <= 9; ++type)
			{
				const auto offset = static_cast<size_t>((type - 4) * 4);
				const auto extra = static_cast<int64_t>(type - 4);
				const auto expected = static_cast<int32_t>(-static_cast<int64_t>(offset + 4) - extra);
				Assert::AreEqual<uint32_t>(static_cast<uint32_t>(expected), get32(image, raw + offset), L"REL32-family displacement");
			}
			Assert::AreEqual<uint32_t>(sectionRva, get32(image, optionalHeader(image) + 16), L"entry remains at section start");
		}

		TEST_METHOD(AbsoluteRelocationsCreateDir64AndHighLowBaseRelocations)
		{
			Workspace workspace;
			TestSection data{ ".data", Bytes(12, 0), 0xc0000040 };
			data.relocations = { { 0, 0, 1 },{ 8, 0, 2 } };
			TestSection text{ ".text",{ 0xc3 } };
			const auto object = makeObject({ data, text }, { TestSymbol{ "main", 0, 2, 2,{} } });
			auto args = outputArgs(workspace);
			args.insert(args.begin() + 1, L"/BASE:0x10000");
			assertSuccess(linkObject(workspace, object, args));
			const auto image = workspace.read(L"linked.exe");
			const auto base = get64(image, optionalHeader(image) + 24);
			const auto dataHeader = sectionHeader(image, ".data");
			const auto dataRva = get32(image, dataHeader + 12);
			const auto dataRaw = get32(image, dataHeader + 20);
			const auto textRva = get32(image, sectionHeader(image, ".text") + 12);
			Assert::AreEqual<uint64_t>(base + textRva, get64(image, dataRaw), L"ADDR64 writes an absolute VA");
			Assert::AreEqual<uint32_t>(static_cast<uint32_t>(base + textRva), get32(image, dataRaw + 8), L"ADDR32 writes a 32-bit VA");
			assertHasBaseReloc(image, dataRva, 10);
			assertHasBaseReloc(image, dataRva + 8, 3);
		}

		TEST_METHOD(Addr32NbWritesAnImageRelativeAddress)
		{
			Workspace workspace;
			TestSection text{ ".text", Bytes(8, 0) };
			text.relocations = { { 0, 0, 3 } };
			const auto object = makeObject({ text }, { TestSymbol{ "main", 4, 1, 2,{} } });
			assertSuccess(linkObject(workspace, object));
			const auto image = workspace.read(L"linked.exe");
			const auto section = sectionHeader(image, ".text");
			Assert::AreEqual<uint32_t>(get32(image, section + 12) + 4, get32(image, get32(image, section + 20)), L"ADDR32NB uses an RVA");
		}

		TEST_METHOD(FixedImageOmitsRelocationsAndMarksThemStripped)
		{
			Workspace workspace;
			TestSection data{ ".data", Bytes(8, 0), 0xc0000040 };
			data.relocations = { { 0, 0, 1 } };
			const auto object = makeObject({ data, TestSection{ ".text",{ 0xc3 } } }, { TestSymbol{ "main", 0, 2, 2,{} } });
			auto args = outputArgs(workspace);
			args.insert(args.begin() + 1, L"/FIXED");
			assertSuccess(linkObject(workspace, object, args));
			const auto image = workspace.read(L"linked.exe");
			Assert::IsTrue((get16(image, 0x96) & 1) != 0, L"relocations-stripped file characteristic");
			Assert::AreEqual<uint32_t>(0, get32(image, optionalHeader(image) + 152), L"fixed image relocation RVA");
			Assert::AreEqual<uint32_t>(0, get32(image, optionalHeader(image) + 156), L"fixed image relocation size");
			Assert::IsTrue((get16(image, optionalHeader(image) + 70) & 0x40) == 0, L"fixed image must not advertise ASLR");
		}

		TEST_METHOD(OrdinaryImportsRemainCallableAtRuntime)
		{
			Workspace workspace;
			TestSection text{ ".text",{ 0xb9, 42, 0, 0, 0, 0xe8, 0, 0, 0, 0, 0xcc } };
			text.relocations = { { 6, 1, 4 } };
			workspace.write(L"input.obj", makeObject({ text }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "ExitProcess", 0, 0, 2,{} } }));
			workspace.write(L"kernel.lib", makeArchive({ { "exit.obj", makeImportObject("ExitProcess", "KERNEL32.dll", "ExitProcess", 0, 4) } }));
			auto args = outputArgs(workspace);
			args.push_back(workspace.file(L"input.obj").wstring());
			args.push_back(workspace.file(L"kernel.lib").wstring());
			assertSuccess(runLinker(workspace, args));
			const auto runtime = runExecutable(workspace, workspace.file(L"linked.exe"), {});
			Assert::AreEqual<DWORD>(42, runtime.exitCode, L"ordinary imports continue to call through their IAT thunks");
		}

		TEST_METHOD(DelayLoadEmitsMetadataAndResolvesFunctionsAtRuntime)
		{
			Workspace workspace;
			TestSection text{ ".text",{ 0xb9, 42, 0, 0, 0, 0xe8, 0, 0, 0, 0, 0xcc } };
			text.relocations = { { 6, 1, 4 } };
			workspace.write(L"input.obj", makeObject({ text, makeDirectiveSection("-delayload:KERNEL32.dll") }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "ExitProcess", 0, 0, 2,{} } }));
			workspace.write(L"kernel.lib", makeArchive({ { "exit.obj", makeImportObject("ExitProcess", "KERNEL32.dll", "ExitProcess", 0, 4) } }));
			auto args = outputArgs(workspace);
			args.push_back(L"/DELAYLOAD:KERNEL32.dll");
			args.push_back(workspace.file(L"input.obj").wstring());
			args.push_back(workspace.file(L"kernel.lib").wstring());
			const auto linkResult = runLinker(workspace, args);
			if (linkResult.exitCode != 0)
			{
				wstring message = L"linker exit code: " + to_wstring(linkResult.exitCode) + L"; output: ";
				message.append(linkResult.output.begin(), linkResult.output.end());
				Assert::Fail(message.c_str());
			}
			const auto image = workspace.read(L"linked.exe");
			const auto optional = optionalHeader(image);
			const auto directoryRva = get32(image, optional + 216);
			Assert::IsTrue(directoryRva != 0, L"delay-import data directory is present");
			Assert::AreEqual<uint32_t>(64, get32(image, optional + 220), L"one delay descriptor plus terminator");
			const auto descriptor = rvaOffset(image, directoryRva);
			Assert::AreEqual<uint32_t>(1, get32(image, descriptor), L"delay descriptor uses RVA-based fields");
			Assert::IsTrue(cStringAtRva(image, get32(image, descriptor + 4)) == "KERNEL32.dll", L"delay descriptor DLL name");
			Assert::AreEqual<uint32_t>(0, get32(image, descriptor + 8), L"the custom resolver does not use a module-handle slot");
			Assert::AreEqual<uint32_t>(0, get32(image, descriptor + 20), L"bound delay IAT is omitted");
			Assert::AreEqual<uint32_t>(0, get32(image, descriptor + 24), L"unload delay IAT is omitted");
			const auto delayIatRva = get32(image, descriptor + 12);
			const auto delayIntRva = get32(image, descriptor + 16);
			const auto delayIat = rvaOffset(image, delayIatRva);
			const auto delayInt = rvaOffset(image, delayIntRva);
			const auto imageBase = get64(image, optional + 24);
			const auto resolverRva = get64(image, delayIat) - imageBase;
			const auto textHeader = sectionHeader(image, ".text");
			Assert::IsTrue(resolverRva >= get32(image, textHeader + 12) && resolverRva < get32(image, textHeader + 12) + get32(image, textHeader + 8), L"initial delay IAT points at generated resolver code");
			Assert::IsTrue(get64(image, delayInt) != 0, L"delay lookup table contains the imported function");
			Assert::AreEqual<uint64_t>(0, get64(image, delayIat + 8), L"delay IAT is null terminated");
			Assert::AreEqual<uint64_t>(0, get64(image, delayInt + 8), L"delay lookup table is null terminated");
			const auto normalImportRva = get32(image, optional + 120);
			Assert::IsTrue(normalImportRva != 0, L"resolver APIs remain ordinary eager imports");
			const auto normalDescriptor = rvaOffset(image, normalImportRva);
			const auto normalLookup = rvaOffset(image, get32(image, normalDescriptor));
			const auto helperNameRva = static_cast<uint32_t>(get64(image, normalLookup));
			Assert::IsTrue(cStringAtRva(image, helperNameRva + 2) == "LoadLibraryA", L"resolver imports LoadLibraryA normally");
			const auto exceptionDirectoryRva = get32(image, optional + 136);
			Assert::IsTrue(exceptionDirectoryRva != 0 && get32(image, optional + 140) == 12, L"resolver has a RUNTIME_FUNCTION unwind record");
			const auto runtimeFunction = rvaOffset(image, exceptionDirectoryRva);
			Assert::AreEqual<uint32_t>(static_cast<uint32_t>(resolverRva), get32(image, runtimeFunction), L"unwind range begins at the resolver");
			Assert::IsTrue(get32(image, runtimeFunction + 4) > get32(image, runtimeFunction), L"unwind range covers resolver code");
			const auto unwindInfo = rvaOffset(image, get32(image, runtimeFunction + 8));
			Assert::AreEqual<uint8_t>(1, image[unwindInfo], L"resolver unwind data uses version 1");
			Assert::AreEqual<uint8_t>(7, image[unwindInfo + 1], L"resolver unwind data describes its stack-allocation prolog");
			const auto runtime = runExecutable(workspace, workspace.file(L"linked.exe"), {});
			Assert::AreEqual<DWORD>(42, runtime.exitCode, L"delay-loaded function resolves on first use");
		}

		TEST_METHOD(DelayLoadRejectsMalformedRequestsAndDataImports)
		{
			Workspace missingWorkspace;
			missingWorkspace.write(L"input.obj", makeEntryObject());
			auto missingArgs = outputArgs(missingWorkspace);
			missingArgs.push_back(L"/DELAYLOAD:missing.dll");
			missingArgs.push_back(missingWorkspace.file(L"input.obj").wstring());
			assertFailureContains(runLinker(missingWorkspace, missingArgs), "no imports from that DLL were selected");

			Workspace optionWorkspace;
			optionWorkspace.write(L"input.obj", makeEntryObject());
			auto optionArgs = outputArgs(optionWorkspace);
			optionArgs.push_back(L"/DELAYLOAD");
			optionArgs.push_back(optionWorkspace.file(L"input.obj").wstring());
			assertFailureContains(runLinker(optionWorkspace, optionArgs), "/DELAYLOAD requires an attached DLL name");

			Workspace directiveWorkspace;
			const auto directiveObject = makeObject({ makeDirectiveSection("-delayload:"), TestSection{ ".text",{ 0xc3 } } }, { TestSymbol{ "main", 0, 2, 2,{} } });
			assertFailureContains(linkObject(directiveWorkspace, directiveObject), ".drectve /DELAYLOAD requires a DLL name");

			Workspace dataWorkspace;
			TestSection data{ ".data", Bytes(8, 0), 0xc0000040 };
			data.relocations = { { 0, 1, 1 } };
			dataWorkspace.write(L"input.obj", makeObject({ data, TestSection{ ".text",{ 0xc3 } } }, { TestSymbol{ "main", 0, 2, 2,{} }, TestSymbol{ "__imp_importedData", 0, 0, 2,{} } }));
			dataWorkspace.write(L"data.lib", makeArchive({ { "data.obj", makeImportObject("importedData", "DATA.dll", "importedData", 17, 1) } }));
			auto dataArgs = outputArgs(dataWorkspace);
			dataArgs.push_back(L"/DELAYLOAD:DATA.dll");
			dataArgs.push_back(dataWorkspace.file(L"input.obj").wstring());
			dataArgs.push_back(dataWorkspace.file(L"data.lib").wstring());
			assertFailureContains(runLinker(dataWorkspace, dataArgs), "cannot be used with data imports");

			Workspace multipleWorkspace;
			TestSection multipleText{ ".text",{ 0xe8, 0, 0, 0, 0, 0xe8, 0, 0, 0, 0, 0xc3 } };
			multipleText.relocations = { { 1, 1, 4 },{ 6, 2, 4 } };
			multipleWorkspace.write(L"input.obj", makeObject({ multipleText }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "GetCurrentProcessId", 0, 0, 2,{} }, TestSymbol{ "ExitProcess", 0, 0, 2,{} } }));
			multipleWorkspace.write(L"kernel.lib", makeArchive({
				{ "pid.obj", makeImportObject("GetCurrentProcessId", "KERNEL32.dll", "GetCurrentProcessId", 0, 4) },
				{ "exit.obj", makeImportObject("ExitProcess", "KERNEL32.dll", "ExitProcess", 0, 4) } }));
			auto multipleArgs = outputArgs(multipleWorkspace);
			multipleArgs.push_back(L"/DELAYLOAD:KERNEL32.dll");
			multipleArgs.push_back(multipleWorkspace.file(L"input.obj").wstring());
			multipleArgs.push_back(multipleWorkspace.file(L"kernel.lib").wstring());
			assertFailureContains(runLinker(multipleWorkspace, multipleArgs), "currently supports one delay-loaded function import per link");
		}

		TEST_METHOD(ImportLibraryGeneratesDescriptorIatAndFunctionThunk)
		{
			Workspace workspace;
			TestSection text{ ".text",{ 0xff, 0x25, 0, 0, 0, 0 } };
			text.relocations = { { 2, 1, 4 } };
			const auto object = makeObject({ text }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "__imp_ExitProcess", 0, 0, 2,{} } });
			workspace.write(L"input.obj", object);
			const auto import = makeImportObject("ExitProcess", "KERNEL32.dll", "ExitProcess", 321, 4);
			workspace.write(L"kernel.lib", makeArchive({ { "exit.obj", import } }));
			auto args = outputArgs(workspace);
			args.push_back(workspace.file(L"input.obj").wstring());
			args.push_back(workspace.file(L"kernel.lib").wstring());
			assertSuccess(runLinker(workspace, args));
			const auto image = workspace.read(L"linked.exe");
			const auto optional = optionalHeader(image);
			const auto importRva = get32(image, optional + 120);
			Assert::IsTrue(importRva != 0, L"import directory RVA");
			const auto descriptor = rvaOffset(image, importRva);
			const auto nameRva = get32(image, descriptor + 12);
			const auto lookupRva = get32(image, descriptor);
			const auto iatRva = get32(image, descriptor + 16);
			Assert::IsTrue(cStringAtRva(image, nameRva) == "KERNEL32.dll", L"import DLL name");
			const auto lookup = rvaOffset(image, lookupRva);
			const auto iat = rvaOffset(image, iatRva);
			Assert::AreEqual<uint64_t>(get64(image, lookup), get64(image, iat), L"lookup and address tables begin identically");
			const auto hintNameRva = static_cast<uint32_t>(get64(image, lookup));
			const auto hintName = rvaOffset(image, hintNameRva);
			Assert::AreEqual<uint16_t>(321, get16(image, hintName), L"import hint");
			Assert::IsTrue(string(reinterpret_cast<const char*>(image.data() + hintName + 2), 11) == "ExitProcess", L"import symbol name");
			Assert::IsTrue(get64(image, lookup + 8) == 0 && get64(image, iat + 8) == 0, L"import thunk tables are null-terminated");
			Assert::IsTrue(get32(image, optional + 208) == iatRva && get32(image, optional + 212) == 16, L"IAT data directory");
		}

		TEST_METHOD(OrdinalImportUsesOrdinalThunkEncoding)
		{
			Workspace workspace;
			TestSection text{ ".text",{ 0xff, 0x25, 0, 0, 0, 0 } };
			text.relocations = { { 2, 1, 4 } };
			workspace.write(L"input.obj", makeObject({ text }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "__imp_OrdinalProc", 0, 0, 2,{} } }));
			workspace.write(L"ordinal.lib", makeArchive({ { "ordinal.obj", makeImportObject("OrdinalProc", "ORDINAL.dll", "", 77, 0) } }));
			auto args = outputArgs(workspace);
			args.push_back(workspace.file(L"input.obj").wstring());
			args.push_back(workspace.file(L"ordinal.lib").wstring());
			assertSuccess(runLinker(workspace, args));
			const auto image = workspace.read(L"linked.exe");
			const auto importRva = get32(image, optionalHeader(image) + 120);
			const auto lookup = rvaOffset(image, get32(image, rvaOffset(image, importRva)));
			Assert::AreEqual<uint64_t>(0x800000000000004dULL, get64(image, lookup), L"ordinal import encoding");
		}

		TEST_METHOD(DataImportCreatesIatWithoutFunctionThunk)
		{
			Workspace workspace;
			TestSection data{ ".data", Bytes(8, 0), 0xc0000040 };
			data.relocations = { { 0, 1, 1 } };
			workspace.write(L"input.obj", makeObject({ data, TestSection{ ".text",{ 0xc3 } } }, { TestSymbol{ "main", 0, 2, 2,{} }, TestSymbol{ "__imp_importedData", 0, 0, 2,{} } }));
			workspace.write(L"data.lib", makeArchive({ { "data.obj", makeImportObject("importedData", "DATA.dll", "importedData", 17, 1) } }));
			auto args = outputArgs(workspace);
			args.push_back(workspace.file(L"input.obj").wstring());
			args.push_back(workspace.file(L"data.lib").wstring());
			assertSuccess(runLinker(workspace, args));
			const auto image = workspace.read(L"linked.exe");
			const auto importRva = get32(image, optionalHeader(image) + 120);
			const auto descriptor = rvaOffset(image, importRva);
			const auto iatRva = get32(image, descriptor + 16);
			Assert::IsTrue(iatRva != 0, L"data import IAT is present");
			Assert::IsTrue(get32(image, optionalHeader(image) + 16) != 0, L"entry RVA remains a valid section-based address");
		}

		TEST_METHOD(MsvcToolchainArchiveIndexesSelectRegularObjectMember)
		{
			Workspace workspace;
			TestSection text{ ".text",{ 0xe8, 0, 0, 0, 0, 0xc3 } };
			text.relocations = { { 1, 1, 4 } };
			workspace.write(L"input.obj", makeObject({ text }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "archiveHelper", 0, 0, 2,{} } }));
			workspace.write(L"toolchain.lib", makeMsvcIndexedArchive());
			auto args = outputArgs(workspace);
			args.push_back(workspace.file(L"input.obj").wstring());
			args.push_back(workspace.file(L"toolchain.lib").wstring());
			assertSuccess(runLinker(workspace, args));
			const auto image = workspace.read(L"linked.exe");
			const auto textHeader = sectionHeader(image, ".text");
			const auto raw = get32(image, textHeader + 20);
			const auto target = static_cast<size_t>(raw) + 5 + get32(image, raw + 1);
			Assert::IsTrue(target < image.size(), L"MSVC archive symbol index resolves the call target within the linked image");
			Assert::AreEqual<uint8_t>(0xc3, image[target], L"toolchain-generated archive member code is included");
		}

		TEST_METHOD(ActualLibrariansSelectLongNamedMembersAndHonorLibraryOrder)
		{
			fs::path toolDirectory;
			error_code ignored;
			wchar_t programFiles[MAX_PATH]{};
			GetEnvironmentVariableW(L"ProgramFiles", programFiles, MAX_PATH);
			for (const auto& entry : fs::recursive_directory_iterator(fs::path(programFiles) / L"Microsoft Visual Studio", fs::directory_options::skip_permission_denied, ignored))
			{
				if (entry.path().filename() == L"lib.exe" && entry.path().parent_path().filename() == L"x64" && entry.path().parent_path().parent_path().filename() == L"Hostx64") toolDirectory = entry.path().parent_path();
			}
			if (toolDirectory.empty()) { Logger::WriteMessage("MSVC x64 librarian not found; actual MSVC archive test skipped"); return; }
			Workspace workspace;
			TestSection text{ ".text",{ 0xe8, 0, 0, 0, 0, 0xc3 } };
			text.relocations = { { 1, 1, 4 } };
			workspace.write(L"input.obj", makeObject({ text }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "helper", 0, 0, 2,{} } }));
			workspace.write(L"long_archive_member_name_first.obj", makeObject({ TestSection{ ".text",{ 0xb8, 37, 0, 0, 0, 0xc3 } } }, { TestSymbol{ "helper", 0, 1, 2,{} } }));
			workspace.write(L"long_archive_member_name_second.obj", makeObject({ TestSection{ ".text",{ 0xb8, 49, 0, 0, 0, 0xc3 } } }, { TestSymbol{ "helper", 0, 1, 2,{} } }));
			workspace.write(L"long_archive_member_name_unused.obj", makeObject({ TestSection{ ".text",{ 0xc3 } } }, { TestSymbol{ "other", 0, 1, 2,{} } }));
			assertSuccess(runExecutable(workspace, toolDirectory / L"lib.exe", { L"/nologo", L"/out:first.lib", L"long_archive_member_name_first.obj", L"long_archive_member_name_unused.obj" }));
			assertSuccess(runExecutable(workspace, toolDirectory / L"lib.exe", { L"/nologo", L"/out:second.lib", L"long_archive_member_name_second.obj" }));
			assertSuccess(runExecutable(workspace, toolPath(L"JWlib.exe"), { L"-q", L"-n", L"-fac", L"jw.lib", L"+long_archive_member_name_first.obj", L"+long_archive_member_name_unused.obj" }, {}, false));
			for (const auto* library : { L"first.lib", L"jw.lib" })
			{
				assertSuccess(runLinker(workspace, { L"/entry:main", L"/out:linked.exe", L"/map", L"input.obj", library }));
				Assert::AreEqual<DWORD>(37, runExecutable(workspace, workspace.file(L"linked.exe"), {}).exitCode, L"actual librarian archive member executes");
				ifstream mapFile(workspace.file(L"linked.map"));
				const string map((istreambuf_iterator<char>(mapFile)), istreambuf_iterator<char>());
				Assert::IsTrue(map.find("long_archive_member_name_first.obj)\n") != string::npos, L"long archive member name is preserved exactly");
				Assert::IsTrue(map.find("long_archive_member_name_unused.obj") == string::npos, L"unused member is not extracted");
				mapFile.close();
				assertSuccess(runLinker(workspace, { L"/entry:main", L"/out:linked.exe", L"/map", L"/include:other", L"input.obj", library }));
				ifstream bothFile(workspace.file(L"linked.map"));
				const string both((istreambuf_iterator<char>(bothFile)), istreambuf_iterator<char>());
				Assert::IsTrue(both.find("long_archive_member_name_unused.obj)\n") != string::npos, L"second long archive name resolves exactly");
			}
			assertSuccess(runLinker(workspace, { L"/entry:main", L"/out:order.exe", L"input.obj", L"second.lib", L"first.lib", L"first.lib" }));
			Assert::AreEqual<DWORD>(49, runExecutable(workspace, workspace.file(L"order.exe"), {}).exitCode, L"first explicit archive wins duplicate symbols");
			assertSuccess(runLinker(workspace, { L"/entry:main", L"/out:default.exe", L"/defaultlib:first.lib", L"/defaultlib:first.lib", L"input.obj", L"second.lib" }));
			Assert::AreEqual<DWORD>(49, runExecutable(workspace, workspace.file(L"default.exe"), {}).exitCode, L"explicit libraries precede repeated default libraries");
		}

		TEST_METHOD(Jwlink2ConsumesJwlibJwasmArchive)
		{
			Workspace workspace;
			workspace.writeText(L"entry.asm", L"option casemap:none\nextern helper:proc\n.code\npublic main\nmain proc\n call helper\n ret\nmain endp\nend\n");
			workspace.writeText(L"helper.asm", L"option casemap:none\n.code\npublic helper\nhelper proc\n mov eax, 37\n ret\nhelper endp\nend\n");
			assertSuccess(runExecutable(workspace, toolPath(L"JWasm.exe"), { L"-win64", L"-Foentry.obj", L"entry.asm" }));
			assertSuccess(runExecutable(workspace, toolPath(L"JWasm.exe"), { L"-win64", L"-Fohelper.obj", L"helper.asm" }));
			assertSuccess(runExecutable(workspace, toolPath(L"JWlib.exe"), { L"-q", L"-n", L"-fac", L"jw.lib", L"+helper.obj" }, {}, false));
			assertSuccess(runLinker(workspace, { L"/entry:main", L"/out:new.exe", L"entry.obj", L"jw.lib" }));
			const auto image = workspace.read(L"new.exe");
			const Bytes marker{ 0xb8, 37, 0, 0, 0, 0xc3 };
			Assert::IsTrue(search(image.begin(), image.end(), marker.begin(), marker.end()) != image.end(), L"selected archive code appears in image");
			Assert::AreEqual<DWORD>(37, runExecutable(workspace, workspace.file(L"new.exe"), {}).exitCode, L"JWasm's empty data section remains loadable");
		}

		TEST_METHOD(WindowsSdkImportLibraryResolvesAndExecutes)
		{
			wchar_t programFiles[MAX_PATH]{};
			GetEnvironmentVariableW(L"ProgramFiles(x86)", programFiles, MAX_PATH);
			const auto root = fs::path(programFiles) / L"Windows Kits" / L"10" / L"Lib";
			fs::path sdkLibrary;
			error_code ignored;
			for (const auto& entry : fs::directory_iterator(root, ignored))
			{
				const auto candidate = entry.path() / L"um" / L"x64" / L"kernel32.lib";
				if (fs::exists(candidate) && (sdkLibrary.empty() || candidate.wstring() > sdkLibrary.wstring())) sdkLibrary = candidate;
			}
			if (sdkLibrary.empty()) { Logger::WriteMessage("Windows SDK x64 kernel32.lib not found; SDK import test skipped"); return; }
			Workspace workspace;
			TestSection text{ ".text",{ 0xb9, 42, 0, 0, 0, 0xe8, 0, 0, 0, 0, 0xcc } };
			text.relocations = { { 6, 1, 4 } };
			workspace.write(L"input.obj", makeObject({ text }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "ExitProcess", 0, 0, 2,{} } }));
			assertSuccess(runLinker(workspace, { L"/entry:main", L"/out:sdk.exe", L"input.obj", sdkLibrary.wstring() }));
			const auto image = workspace.read(L"sdk.exe");
			const auto descriptor = rvaOffset(image, get32(image, optionalHeader(image) + 120));
			Assert::IsTrue(cStringAtRva(image, get32(image, descriptor + 12)) == "KERNEL32.dll", L"SDK import descriptor names KERNEL32.dll");
			Assert::AreEqual<DWORD>(42, runExecutable(workspace, workspace.file(L"sdk.exe"), {}).exitCode, L"SDK import executes through the Windows loader");
		}

		TEST_METHOD(MsvcAndJwlink2ImportLibrariesInteroperate)
		{
			fs::path toolDirectory;
			error_code ignored;
			wchar_t programFiles[MAX_PATH]{};
			GetEnvironmentVariableW(L"ProgramFiles", programFiles, MAX_PATH);
			for (const auto& entry : fs::recursive_directory_iterator(fs::path(programFiles) / L"Microsoft Visual Studio", fs::directory_options::skip_permission_denied, ignored))
			{
				if (entry.path().filename() == L"lib.exe" && entry.path().parent_path().filename() == L"x64" && entry.path().parent_path().parent_path().filename() == L"Hostx64" && fs::exists(entry.path().parent_path() / L"link.exe")) toolDirectory = entry.path().parent_path();
			}
			if (toolDirectory.empty()) { Logger::WriteMessage("MSVC x64 tools not found; import-library interoperability test skipped"); return; }
			for (const auto* dll : { L"fixture.dll", L"fixture_with_a_long_module_name.dll" })
			{
				Workspace workspace;
				workspace.write(L"dll.obj", makeObject({ TestSection{ ".text",{ 0xb8, 37, 0, 0, 0, 0xc3, 0xb8, 5, 0, 0, 0, 0xc3 } }, TestSection{ ".data",{ 7, 0, 0, 0 }, 0xc0000040 } }, { TestSymbol{ "api", 0, 1, 2,{} }, TestSymbol{ "ordinalFn", 6, 1, 2,{} }, TestSymbol{ "value", 0, 2, 2,{} } }));
				assertSuccess(runLinker(workspace, { L"/dll", L"/noentry", L"/base:0x180000000", L"/export:api", L"/export:ordinalFn,@5,NONAME", L"/export:value,DATA", wstring(L"/out:") + dll, L"/implib:jw2.lib", L"dll.obj" }));
				workspace.writeText(L"fixture.def", wstring(L"LIBRARY ") + dll + L"\nEXPORTS\n api\n ordinalFn @5 NONAME\n value DATA\n");
				assertSuccess(runExecutable(workspace, toolDirectory / L"lib.exe", { L"/nologo", L"/machine:x64", L"/def:fixture.def", L"/out:msvc.lib" }));
				TestSection text{ ".text",{ 0xe8, 0, 0, 0, 0, 0xc3 } };
				text.relocations = { { 1, 1, 4 } };
				workspace.write(L"client.obj", makeObject({ text }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "api", 0, 0, 2,{} } }));
				assertSuccess(runLinker(workspace, { L"/entry:main", L"/out:jw2-client.exe", L"client.obj", L"msvc.lib" }));
				Assert::AreEqual<DWORD>(37, runExecutable(workspace, workspace.file(L"jw2-client.exe"), {}).exitCode, L"JWlink2 consumes MSVC import library");
				if (wstring(dll) == L"fixture.dll")
				{
					assertSuccess(runExecutable(workspace, toolPath(L"JWlib.exe"), { L"-q", L"-n", L"-fac", L"-i6", L"-ic", L"jw.lib", L"++api.fixture.api" }, {}, false));
					assertSuccess(runLinker(workspace, { L"/entry:main", L"/out:jwlib-client.exe", L"client.obj", L"jw.lib" }));
					Assert::AreEqual<DWORD>(37, runExecutable(workspace, workspace.file(L"jwlib-client.exe"), {}).exitCode, L"JWlink2 consumes JWlib AMD64 import archive");
					workspace.write(L"other.obj", makeObject({ TestSection{ ".text",{ 0xb8, 49, 0, 0, 0, 0xc3 } } }, { TestSymbol{ "otherApi", 0, 1, 2,{} } }));
					assertSuccess(runLinker(workspace, { L"/dll", L"/noentry", L"/base:0x190000000", L"/export:otherApi", L"/out:other.dll", L"/implib:other.lib", L"other.obj" }));
					TestSection pair{ ".text",{ 0xe8, 0, 0, 0, 0, 0xe9, 0, 0, 0, 0 } };
					pair.relocations = { { 1, 1, 4 }, { 6, 2, 4 } };
					workspace.write(L"pair.obj", makeObject({ pair }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "api", 0, 0, 2,{} }, TestSymbol{ "otherApi", 0, 0, 2,{} } }));
					assertSuccess(runExecutable(workspace, toolDirectory / L"link.exe", { L"/nologo", L"/nodefaultlib", L"/subsystem:console", L"/entry:main", L"/out:pair.exe", L"pair.obj", L"jw2.lib", L"other.lib" }));
					Assert::AreEqual<DWORD>(49, runExecutable(workspace, workspace.file(L"pair.exe"), {}).exitCode, L"MSVC LINK resolves generated libraries for two DLLs");
				}
				assertSuccess(runExecutable(workspace, toolDirectory / L"link.exe", { L"/nologo", L"/nodefaultlib", L"/subsystem:console", L"/entry:main", L"/out:msvc-client.exe", L"client.obj", L"jw2.lib" }));
				Assert::AreEqual<DWORD>(37, runExecutable(workspace, workspace.file(L"msvc-client.exe"), {}).exitCode, L"MSVC LINK consumes JWlink2 import library");
				TestSection mixed{ ".text",{ 0xe8, 0, 0, 0, 0, 0x48, 0x8b, 0x05, 0, 0, 0, 0, 0x8b, 0x00, 0xc3 } };
				mixed.relocations = { { 1, 1, 4 }, { 8, 2, 4 } };
				workspace.write(L"mixed.obj", makeObject({ mixed }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "ordinalFn", 0, 0, 2,{} }, TestSymbol{ "__imp_value", 0, 0, 2,{} } }));
				for (const auto* library : { L"jw2.lib", L"msvc.lib" })
				{
					assertSuccess(runExecutable(workspace, toolDirectory / L"link.exe", { L"/nologo", L"/nodefaultlib", L"/subsystem:console", L"/entry:main", L"/out:mixed.exe", L"mixed.obj", library }));
					Assert::AreEqual<DWORD>(7, runExecutable(workspace, workspace.file(L"mixed.exe"), {}).exitCode, L"MSVC LINK resolves ordinal and data imports");
					assertSuccess(runLinker(workspace, { L"/entry:main", L"/out:mixed-jw2.exe", L"mixed.obj", library }));
					Assert::AreEqual<DWORD>(7, runExecutable(workspace, workspace.file(L"mixed-jw2.exe"), {}).exitCode, L"JWlink2 resolves ordinal and data imports");
				}
			}
		}

		TEST_METHOD(ArchiveLongNameRequiresTerminator)
		{
			Workspace workspace;
			Bytes archive{ '!', '<', 'a', 'r', 'c', 'h', '>', '\n' };
			appendArchiveMember(archive, "//", Bytes{ 'b', 'a', 'd' });
			appendArchiveMember(archive, "/0", makeEntryObject());
			workspace.write(L"bad.lib", archive);
			assertFailureContains(runLinker(workspace, { L"/entry:main", L"/out:bad.exe", L"bad.lib" }), "unterminated or empty archive long name");
		}

		TEST_METHOD(MalformedMsvcArchiveIndexIsRejected)
		{
			Workspace workspace;
			auto archive = makeMsvcIndexedArchive();
			archive.at(68) = 0xff;
			archive.at(69) = 0xff;
			archive.at(70) = 0xff;
			archive.at(71) = 0xff;
			workspace.write(L"malformed.lib", archive);
			auto args = outputArgs(workspace);
			args.push_back(workspace.file(L"malformed.lib").wstring());
			assertFailureContains(runLinker(workspace, args), "invalid first archive linker-member symbol count");
		}

		TEST_METHOD(StaticArchiveExtractsOnlyNeededObjectMember)
		{
			Workspace workspace;
			TestSection text{ ".text", Bytes(4, 0) };
			text.relocations = { { 0, 1, 4 } };
			workspace.write(L"input.obj", makeObject({ text }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "helper", 0, 0, 2,{} } }));
			workspace.write(L"helpers.lib", makeArchive({ { "helper.obj", makeObject({ TestSection{ ".text",{ 0xc3 } } },{ TestSymbol{ "helper", 0, 1, 2,{} } }) } }));
			auto args = outputArgs(workspace);
			args.push_back(workspace.file(L"input.obj").wstring());
			args.push_back(workspace.file(L"helpers.lib").wstring());
			assertSuccess(runLinker(workspace, args));
			Assert::IsTrue(sectionHeader(workspace.read(L"linked.exe"), ".text") != 0, L"archive member section should be linked");
		}

		TEST_METHOD(DefaultLibraryAndLibraryPathResolveArchive)
		{
			Workspace workspace;
			TestSection text{ ".text", Bytes(4, 0) };
			text.relocations = { { 0, 1, 4 } };
			workspace.write(L"input.obj", makeObject({ text }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "helper", 0, 0, 2,{} } }));
			fs::create_directories(workspace.path / L"lib");
			ofstream library(workspace.path / L"lib" / L"default.lib", ios::binary);
			const auto archive = makeArchive({ { "helper.obj", makeObject({ TestSection{ ".text",{ 0xc3 } } },{ TestSymbol{ "helper", 0, 1, 2,{} } }) } });
			library.write(reinterpret_cast<const char*>(archive.data()), static_cast<streamsize>(archive.size()));
			library.close();
			auto args = outputArgs(workspace);
			args.insert(args.begin() + 1, L"/DEFAULTLIB:default");
			args.insert(args.begin() + 1, L"/LIBPATH:" + (workspace.path / L"lib").wstring());
			args.push_back(workspace.file(L"input.obj").wstring());
			assertSuccess(runLinker(workspace, args));
		}

		TEST_METHOD(DefaultLibraryExclusionsDoNotSuppressExplicitLibraries)
		{
			Workspace workspace;
			TestSection text{ ".text", Bytes(4, 0) };
			text.relocations = { { 0, 1, 4 } };
			const auto object = makeObject({ text, makeDirectiveSection("-defaultlib:helpers.lib") }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "helper", 0, 0, 2,{} } });
			workspace.write(L"input.obj", object);
			workspace.write(L"helpers.lib", makeArchive({ { "helper.obj", makeObject({ TestSection{ ".text",{ 0xc3 } } }, { TestSymbol{ "helper", 0, 1, 2,{} } }) } }));
			for (const auto* exclusion : { L"/NODEFAULTLIB", L"/NODEFAULTLIB:helpers" })
			{
				const vector<wstring> args{ L"/ENTRY:main", L"/OUT:excluded.exe", exclusion, L"input.obj" };
				const auto result = runLinker(workspace, args);
				assertFailureContains(result, "helper");
				Assert::AreEqual<DWORD>(1, result.exitCode);
				Assert::IsFalse(fs::exists(workspace.file(L"excluded.exe")));
				assertSuccess(runLinker(workspace, { L"/ENTRY:main", L"/OUT:explicit.exe", exclusion, L"input.obj", L"helpers.lib" }));
			}
		}

		TEST_METHOD(CoffDirectiveEntryPointIsHonoredAndDirectiveSectionIsConsumed)
		{
			Workspace workspace;
			const auto object = makeObject(
				{ TestSection{ ".text",{ 0xc3 } }, makeDirectiveSection("-entry:mainCRTStartup ") },
				{ TestSymbol{ "mainCRTStartup", 0, 1, 2,{} } });
			workspace.write(L"input.obj", object);
			assertSuccess(runLinker(workspace, { L"/OUT:" + workspace.file(L"linked.exe").wstring(), workspace.file(L"input.obj").wstring() }));
			const auto image = workspace.read(L"linked.exe");
			Assert::AreEqual<uint32_t>(0x1000, get32(image, optionalHeader(image) + 16), L".drectve entry selects the startup symbol");
			Assert::IsFalse(hasSection(image, ".drectve"), L"linker directives are not emitted as an image section");
		}

		TEST_METHOD(CoffDefaultLibraryDirectiveLoadsLibraryWithQuotedName)
		{
			Workspace workspace;
			TestSection text{ ".text", Bytes(4, 0) };
			text.relocations = { { 0, 1, 4 } };
			const auto object = makeObject({ text, makeDirectiveSection("-defaultlib:\"helpers.lib\"") },
				{ TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "helper", 0, 0, 2,{} } });
			workspace.write(L"input.obj", object);
			workspace.write(L"helpers.lib", makeArchive({ { "helper.obj", makeObject({ TestSection{ ".text",{ 0xc3 } } },{ TestSymbol{ "helper", 0, 1, 2,{} } }) } }));
			assertSuccess(runLinker(workspace, { L"/OUT:" + workspace.file(L"linked.exe").wstring(), L"/ENTRY:main", workspace.file(L"input.obj").wstring() }));
			Assert::IsTrue(hasSection(workspace.read(L"linked.exe"), ".text"), L"default-library member was linked");
		}

		TEST_METHOD(CoffDirectivesFromSelectedArchiveMembersAreProcessed)
		{
			Workspace workspace;
			TestSection entryText{ ".text", Bytes(4, 0) };
			entryText.relocations = { { 0, 1, 4 } };
			workspace.write(L"input.obj", makeObject({ entryText }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "helper", 0, 0, 2,{} } }));
			TestSection helperText{ ".text", Bytes(4, 0) };
			helperText.relocations = { { 0, 1, 4 } };
			const auto helper = makeObject({ helperText, makeDirectiveSection("-defaultlib:secondary.lib") },
				{ TestSymbol{ "helper", 0, 1, 2,{} }, TestSymbol{ "secondarySymbol", 0, 0, 2,{} } });
			const auto unused = makeObject({ makeDirectiveSection("-unknown:ignored-unless-selected") }, { TestSymbol{ "unusedSymbol", 0, 0, 2,{} } });
			workspace.write(L"outer.lib", makeArchive({ { "helper.obj", helper },{ "unused.obj", unused } }));
			workspace.write(L"secondary.lib", makeArchive({ { "secondary.obj", makeObject({ TestSection{ ".data",{ 0x6b }, 0xc0000040 } },{ TestSymbol{ "secondarySymbol", 0, 1, 2,{} } }) } }));
			auto args = outputArgs(workspace);
			args.push_back(workspace.file(L"input.obj").wstring());
			args.push_back(workspace.file(L"outer.lib").wstring());
			assertSuccess(runLinker(workspace, args));
			const auto image = workspace.read(L"linked.exe");
			const auto data = sectionHeader(image, ".data");
			Assert::AreEqual<uint8_t>(0x6b, image[get32(image, data + 20)], L"selected member default library is loaded and unused member directives are ignored");
		}

		TEST_METHOD(CoffIncludeDirectiveForcesArchiveMemberExtraction)
		{
			Workspace workspace;
			const auto object = makeObject({ TestSection{ ".text",{ 0xc3 } }, makeDirectiveSection("-include:forcedSymbol") },
				{ TestSymbol{ "main", 0, 1, 2,{} } });
			workspace.write(L"input.obj", object);
			workspace.write(L"forced.lib", makeArchive({ { "forced.obj", makeObject({ TestSection{ ".data",{ 0x5a }, 0xc0000040 } },{ TestSymbol{ "forcedSymbol", 0, 1, 2,{} } }) } }));
			assertSuccess(runLinker(workspace, { L"/OUT:" + workspace.file(L"linked.exe").wstring(), L"/ENTRY:main", workspace.file(L"input.obj").wstring(), workspace.file(L"forced.lib").wstring() }));
			const auto image = workspace.read(L"linked.exe");
			const auto data = sectionHeader(image, ".data");
			Assert::AreEqual<uint8_t>(0x5a, image[get32(image, data + 20)], L"forced archive member contributes its data");
		}

		TEST_METHOD(MapReportsDeterministicImageAndArchiveContributions)
		{
			Workspace workspace;
			workspace.write(L"input.obj", makeObject({ TestSection{ ".text",{ 0xc3 } } },
				{ TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "zeta", 0, 1, 2,{} }, TestSymbol{ "alpha", 0, 1, 2,{} } }));
			workspace.write(L"forced.lib", makeArchive({
				{ "forced.obj", makeObject({ TestSection{ ".data",{ 0x5a }, 0xc0000040 } }, { TestSymbol{ "forced", 0, 1, 2,{} } }) },
				{ "unused.obj", makeObject({ TestSection{ ".data",{ 0x33 }, 0xc0000040 } }, { TestSymbol{ "unused", 0, 1, 2,{} } }) } }));
			auto args = outputArgs(workspace);
			args.insert(args.begin() + 1, { L"/MAP", L"/INCLUDE:forced" });
			args.push_back(workspace.file(L"input.obj").wstring());
			args.push_back(workspace.file(L"forced.lib").wstring());
			assertSuccess(runLinker(workspace, args));
			auto readMap = [&]() { ifstream map(workspace.file(L"linked.map")); return string((istreambuf_iterator<char>(map)), istreambuf_iterator<char>()); };
			const auto contents = readMap();
			const auto image = workspace.read(L"linked.exe");
			const auto base = get64(image, optionalHeader(image) + 24);
			const auto text = sectionHeader(image, ".text");
			const auto memory = contents.find("\nMemory Map\nAddress Symbol\n");
			const auto contributions = contents.find("\nInput Contributions\nAddress Size Section Module\n");
			Assert::IsTrue(memory != string::npos && contributions != string::npos && memory < contributions, L"map tables are present");
			const auto sectionRow = contents.find(" .text\n");
			Assert::IsTrue(sectionRow != string::npos && sectionRow < memory, L"output section row is present");
			const auto sectionStart = contents.rfind('\n', sectionRow) + 1;
			const auto sizeStart = contents.find(' ', sectionStart) + 1;
			Assert::AreEqual<uint64_t>(base + get32(image, text + 12), stoull(contents.substr(sectionStart, sizeStart - sectionStart), nullptr, 16), L"section address matches PE header");
			Assert::AreEqual<uint64_t>(get32(image, text + 8), stoull(contents.substr(sizeStart, sectionRow - sizeStart), nullptr, 16), L"section size matches PE header");
			const auto alpha = contents.find(" alpha\n", memory);
			const auto main = contents.find(" main\n", memory);
			const auto zeta = contents.find(" zeta\n", memory);
			Assert::IsTrue(alpha < main && main < zeta && zeta < contributions, L"equal-address symbols are sorted by name");
			const auto line = contents.rfind('\n', alpha);
			Assert::AreEqual<uint64_t>(base + get32(image, text + 12), stoull(contents.substr(line + 1, alpha - line - 1), nullptr, 16), L"symbol address matches PE layout");
			Assert::IsTrue(contents.find("forced.lib(forced.obj)", contributions) != string::npos, L"selected archive member is reported");
			Assert::IsTrue(contents.find("unused.obj", contributions) == string::npos, L"unselected archive member is absent");
			assertSuccess(runLinker(workspace, args));
			Assert::IsTrue(contents == readMap(), L"map output is deterministic across links");

			workspace.write(L"reference.obj", makeEntryObject());
			assertSuccess(runExecutable(workspace, jwlinkPath(), { L"format", L"win", L"pe", L"option", L"quiet,start=main,map=" + workspace.file(L"reference.map").wstring(), L"file", workspace.file(L"reference.obj").wstring(), L"name", workspace.file(L"reference.exe").wstring() }));
			ifstream referenceFile(workspace.file(L"reference.map"));
			const string reference((istreambuf_iterator<char>(referenceFile)), istreambuf_iterator<char>());
			Assert::IsTrue(reference.find("Groups") != string::npos && reference.find("Segments") != string::npos && reference.find("Memory Map") != string::npos && reference.find("main") != string::npos, L"JWlink map uses its own group, segment and symbol tables");
		}

		TEST_METHOD(CoffAlignDirectiveSetsSectionAndFileAlignment)
		{
			Workspace workspace;
			const auto object = makeObject({ TestSection{ ".text",{ 0xc3 } }, makeDirectiveSection("-ALIGN:0x200") },
				{ TestSymbol{ "main", 0, 1, 2,{} } });
			assertSuccess(linkObject(workspace, object));
			const auto image = workspace.read(L"linked.exe");
			const auto optional = optionalHeader(image);
			Assert::AreEqual<uint32_t>(0x200, get32(image, optional + 32), L"directive sets section alignment");
			Assert::AreEqual<uint32_t>(0x200, get32(image, optional + 36), L"small section alignment also sets file alignment");
			Assert::AreEqual<uint32_t>(0x200, get32(image, sectionHeader(image, ".text") + 12), L"first section RVA obeys alignment");

			Workspace commandLineWorkspace;
			const auto overridden = makeObject({ TestSection{ ".text",{ 0xc3 } }, makeDirectiveSection("-ALIGN:0x200") },
				{ TestSymbol{ "main", 0, 1, 2,{} } });
			commandLineWorkspace.write(L"input.obj", overridden);
			auto args = outputArgs(commandLineWorkspace);
			args.insert(args.begin() + 1, L"/ALIGN:0x1000");
			args.push_back(commandLineWorkspace.file(L"input.obj").wstring());
			assertSuccess(runLinker(commandLineWorkspace, args));
			const auto overriddenImage = commandLineWorkspace.read(L"linked.exe");
			Assert::AreEqual<uint32_t>(0x1000, get32(overriddenImage, optionalHeader(overriddenImage) + 32), L"command-line alignment overrides the object directive");
		}

		TEST_METHOD(CoffAlignCommDirectiveAlignsCommonSymbol)
		{
			Workspace workspace;
			const auto object = makeObject({ TestSection{ ".text",{ 0xc3 } }, makeDirectiveSection("-aligncomm:alignedCommon,32") },
				{ TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "alignedCommon", 4, 0, 2,{} } });
			workspace.write(L"input.obj", object);
			auto args = outputArgs(workspace);
			args.insert(args.begin() + 1, L"/MAP");
			args.push_back(workspace.file(L"input.obj").wstring());
			assertSuccess(runLinker(workspace, args));
			ifstream map(workspace.file(L"linked.map"));
			const string contents((istreambuf_iterator<char>(map)), istreambuf_iterator<char>());
			const auto symbolPosition = contents.find(" alignedCommon\n");
			Assert::IsTrue(symbolPosition != string::npos, L"aligned common symbol is present in the map");
			const auto lineStart = contents.rfind('\n', symbolPosition);
			const auto addressStart = lineStart == string::npos ? 0 : lineStart + 1;
			const auto addressEnd = contents.find(' ', addressStart);
			const auto address = stoull(contents.substr(addressStart, addressEnd - addressStart), nullptr, 16);
			Assert::AreEqual<uint64_t>(0, address % 32, L"ALIGNCOMM address respects the requested alignment");
		}

		TEST_METHOD(CoffExportDirectiveBuildsExportDirectoryAndAlias)
		{
			Workspace workspace;
			const auto object = makeObject({ TestSection{ ".text",{ 0xc3, 0xc3 } }, makeDirectiveSection("-export:api=implementation,@7") },
				{ TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "implementation", 1, 1, 2,{} } });
			assertSuccess(linkObject(workspace, object));
			const auto image = workspace.read(L"linked.exe");
			const auto optional = optionalHeader(image);
			const auto exportRva = get32(image, optional + 112);
			Assert::IsTrue(exportRva != 0 && get32(image, optional + 116) >= 40, L"export data directory is present");
			const auto directory = rvaOffset(image, exportRva);
			Assert::AreEqual<uint32_t>(7, get32(image, directory + 16), L"export ordinal base");
			Assert::AreEqual<uint32_t>(1, get32(image, directory + 20), L"number of exported functions");
			Assert::AreEqual<uint32_t>(1, get32(image, directory + 24), L"number of named exports");
			const auto functions = rvaOffset(image, get32(image, directory + 28));
			const auto names = rvaOffset(image, get32(image, directory + 32));
			const auto ordinals = rvaOffset(image, get32(image, directory + 36));
			const auto text = sectionHeader(image, ".text");
			Assert::AreEqual<uint32_t>(get32(image, text + 12) + 1, get32(image, functions), L"alias resolves to the internal symbol RVA");
			Assert::IsTrue(cStringAtRva(image, get32(image, names)) == "api", L"public export name");
			Assert::AreEqual<uint16_t>(0, get16(image, ordinals), L"export name maps to ordinal index zero");
		}

		TEST_METHOD(CommonSymbolAllocatesBssAndAppearsInMap)
		{
			Workspace workspace;
			const auto object = makeObject({ TestSection{ ".text",{ 0xc3 } } }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "commonValue", 32, 0, 2,{} } });
			workspace.write(L"input.obj", object);
			auto args = outputArgs(workspace);
			args.insert(args.begin() + 1, L"/MAP");
			args.push_back(workspace.file(L"input.obj").wstring());
			assertSuccess(runLinker(workspace, args));
			const auto image = workspace.read(L"linked.exe");
			const auto bss = sectionHeader(image, ".bss");
			Assert::IsTrue(get32(image, bss + 8) >= 32, L"common symbol storage is allocated in BSS");
			ifstream map(workspace.file(L"linked.map"));
			const string contents((istreambuf_iterator<char>(map)), istreambuf_iterator<char>());
			Assert::IsTrue(contents.find("commonValue") != string::npos, L"map includes common symbol");
		}

		TEST_METHOD(CommonSymbolsUseLargestSizeAndStrongDefinitionsWin)
		{
			Workspace commonWorkspace;
			TestSection references{ ".data", Bytes(16, 0), 0xc0000040 };
			references.relocations = { { 0, 1, 1 },{ 8, 2, 1 } };
			commonWorkspace.write(L"entry.obj", makeObject({ references, TestSection{ ".text",{ 0xc3 } } }, {
				TestSymbol{ "main", 0, 2, 2,{} }, TestSymbol{ "firstCommon", 4, 0, 2,{} }, TestSymbol{ "secondCommon", 8, 0, 2,{} },
				TestSymbol{ "firstCommon", 16, 0, 2,{} }
				}));
			auto commonArgs = outputArgs(commonWorkspace);
			commonArgs.push_back(commonWorkspace.file(L"entry.obj").wstring());
			assertSuccess(runLinker(commonWorkspace, commonArgs));
			auto verifyCommonRanges = [&](const Bytes& image)
				{
					const auto data = get32(image, sectionHeader(image, ".data") + 20);
					const auto first = get64(image, data);
					const auto second = get64(image, data + 8);
					Assert::IsTrue(first + 16 <= second || second + 8 <= first, L"merged common symbols occupy disjoint storage using the largest common size");
				};
			verifyCommonRanges(commonWorkspace.read(L"linked.exe"));

			Workspace strongWorkspace;
			TestSection strongReference{ ".data", Bytes(8, 0), 0xc0000040 };
			strongReference.relocations = { { 0, 1, 1 } };
			strongWorkspace.write(L"entry.obj", makeObject({ strongReference, TestSection{ ".text",{ 0xc3 } } }, {
				TestSymbol{ "main", 0, 2, 2,{} }, TestSymbol{ "sharedValue", 32, 0, 2,{} }
				}));
			strongWorkspace.write(L"strong.obj", makeObject({ TestSection{ ".data",{ 0x5a }, 0xc0000040 } }, { TestSymbol{ "sharedValue", 0, 1, 2,{} } }));
			auto strongArgs = outputArgs(strongWorkspace);
			strongArgs.push_back(strongWorkspace.file(L"entry.obj").wstring());
			strongArgs.push_back(strongWorkspace.file(L"strong.obj").wstring());
			assertSuccess(runLinker(strongWorkspace, strongArgs));
			auto verifyStrongOverride = [&](const Bytes& image)
				{
					const auto optional = optionalHeader(image);
					const auto base = get64(image, optional + 24);
					const auto strongData = sectionHeader(image, ".data");
					const auto raw = get32(image, strongData + 20);
					const auto strongAddress = get64(image, raw);
					Assert::AreEqual<uint64_t>(base + get32(image, strongData + 12) + 8, strongAddress, L"a strong definition overrides a common symbol regardless of object order");
					Assert::AreEqual<uint8_t>(0x5a, image[raw + 8], L"strong definition bytes are linked");
				};
			verifyStrongOverride(strongWorkspace.read(L"linked.exe"));
		}

		TEST_METHOD(ComdatAnyKeepsOneDuplicateDefinition)
		{
			Workspace workspace;
			workspace.write(L"entry.obj", makeEntryObject());
			workspace.write(L"first.obj", makeComdatObject("shared", { 0x11 }, 2));
			workspace.write(L"second.obj", makeComdatObject("shared", { 0x22 }, 2));
			auto args = outputArgs(workspace);
			args.insert(args.begin() + 1, L"/MAP");
			for (const auto& file : { L"entry.obj", L"first.obj", L"second.obj" }) args.push_back(workspace.file(file).wstring());
			assertSuccess(runLinker(workspace, args));
			assertSuccess(runJwlink(workspace, { workspace.file(L"entry.obj").wstring(), workspace.file(L"first.obj").wstring(), workspace.file(L"second.obj").wstring() }, L"jwlink.exe"));
			auto verifyWinner = [&](const Bytes& image)
				{
					const auto text = sectionHeader(image, ".text");
					const auto raw = get32(image, text + 20);
					const auto end = raw + get32(image, text + 16);
					Assert::IsTrue(find(image.begin() + raw, image.begin() + end, 0x11) != image.begin() + end, L"first ANY COMDAT candidate is selected");
					Assert::IsTrue(find(image.begin() + raw, image.begin() + end, 0x22) == image.begin() + end, L"losing ANY COMDAT candidate is discarded");
				};
			verifyWinner(workspace.read(L"linked.exe"));
			verifyWinner(workspace.read(L"jwlink.exe"));
			ifstream map(workspace.file(L"linked.map"));
			const string contents((istreambuf_iterator<char>(map)), istreambuf_iterator<char>());
			const auto first = contents.find(" shared\n");
			Assert::IsTrue(first != string::npos && contents.find(" shared\n", first + 1) == string::npos, L"map contains exactly one selected COMDAT symbol");
		}

		TEST_METHOD(ComdatLargestChoosesLargestCandidate)
		{
			Workspace workspace;
			workspace.write(L"entry.obj", makeEntryObject());
			workspace.write(L"small.obj", makeComdatObject("largest", { 0x10 }, 6));
			workspace.write(L"large.obj", makeComdatObject("largest", { 0x20, 0x21, 0x22 }, 6));
			auto args = outputArgs(workspace);
			args.insert(args.begin() + 1, L"/MAP");
			for (const auto& file : { L"entry.obj", L"small.obj", L"large.obj" }) args.push_back(workspace.file(file).wstring());
			assertSuccess(runLinker(workspace, args));
			const auto image = workspace.read(L"linked.exe");
			const auto text = sectionHeader(image, ".text");
			const auto raw = get32(image, text + 20);
			const auto rawSize = get32(image, text + 16);
			const Bytes expected{ 0x20, 0x21, 0x22 };
			Assert::IsTrue(search(image.begin() + raw, image.begin() + raw + rawSize, expected.begin(), expected.end()) != image.begin() + raw + rawSize,
				L"largest COMDAT bytes are retained");
		}

		TEST_METHOD(ComdatNewestChoosesLastCandidate)
		{
			Workspace workspace;
			workspace.write(L"entry.obj", makeEntryObject());
			workspace.write(L"old.obj", makeComdatObject("newest", { 0x31 }, 7));
			workspace.write(L"new.obj", makeComdatObject("newest", { 0x32 }, 7));
			auto args = outputArgs(workspace);
			for (const auto& file : { L"entry.obj", L"old.obj", L"new.obj" }) args.push_back(workspace.file(file).wstring());
			assertSuccess(runLinker(workspace, args));
			const auto image = workspace.read(L"linked.exe");
			const auto text = sectionHeader(image, ".text");
			const auto raw = get32(image, text + 20);
			Assert::IsTrue(find(image.begin() + raw, image.begin() + raw + get32(image, text + 16), 0x32) != image.begin() + raw + get32(image, text + 16), L"NEWEST COMDAT data is selected");
		}

		TEST_METHOD(AssociativeComdatFollowsSelectedParent)
		{
			Workspace workspace;
			workspace.write(L"entry.obj", makeEntryObject());
			workspace.write(L"parent1.obj", makeAssociativeComdatObject({ 0xc3 }, { 0x41, 0x42 }));
			workspace.write(L"parent2.obj", makeAssociativeComdatObject({ 0xc3 }, { 0x51, 0x52 }));
			auto args = outputArgs(workspace);
			for (const auto& file : { L"entry.obj", L"parent1.obj", L"parent2.obj" }) args.push_back(workspace.file(file).wstring());
			assertSuccess(runLinker(workspace, args));
			const auto image = workspace.read(L"linked.exe");
			const auto xdata = sectionHeader(image, ".xdata");
			const auto raw = get32(image, xdata + 20);
			Assert::AreEqual<uint8_t>(0x41, image[raw], L"associative section follows the selected first parent");
			Assert::AreEqual<uint8_t>(0x42, image[raw + 1], L"associative section data is retained");
			Assert::IsTrue(find(image.begin() + raw, image.begin() + raw + get32(image, xdata + 16), 0x51) == image.begin() + raw + get32(image, xdata + 16), L"losing parent's associative section is discarded");
		}

		TEST_METHOD(WeakExternalResolvesFallbackFromLibrary)
		{
			Workspace workspace;
			TestSection text{ ".text", Bytes(8, 0) };
			text.relocations = { { 0, 1, 1 } };
			TestSymbol weak{ "weakRef", 0, 0, 105,{ weakAux(3, 3) } };
			TestSymbol fallback{ "fallback", 0, 0, 2,{} };
			workspace.write(L"input.obj", makeObject({ text }, { TestSymbol{ "main", 0, 1, 2,{} }, weak, fallback }));
			workspace.write(L"fallback.lib", makeArchive({ { "fallback.obj", makeObject({ TestSection{ ".data",{ 0x77 }, 0xc0000040 } },{ TestSymbol{ "fallback", 0, 1, 2,{} } }) } }));
			auto args = outputArgs(workspace);
			args.push_back(workspace.file(L"input.obj").wstring());
			args.push_back(workspace.file(L"fallback.lib").wstring());
			assertSuccess(runLinker(workspace, args));
			const auto image = workspace.read(L"linked.exe");
			const auto target = get64(image, get32(image, sectionHeader(image, ".text") + 20));
			const auto data = sectionHeader(image, ".data");
			const auto expected = get64(image, optionalHeader(image) + 24) + get32(image, data + 12);
			Assert::AreEqual<uint64_t>(expected, target, L"weak alias relocates to the selected library fallback");
			Assert::AreEqual<uint8_t>(0x77, image[get32(image, data + 20)], L"fallback member data is included");
		}

		TEST_METHOD(WeakSearchNoneResolvesToZero)
		{
			Workspace workspace;
			TestSection text{ ".text", Bytes(8, 0) };
			text.relocations = { { 0, 1, 1 } };
			TestSymbol weak{ "optional", 0, 0, 105,{ weakAux(3, 1) } };
			TestSymbol fallback{ "missingFallback", 0, 0, 2,{} };
			const auto object = makeObject({ text }, { TestSymbol{ "main", 0, 1, 2,{} }, weak, fallback });
			assertSuccess(linkObject(workspace, object));
			const auto image = workspace.read(L"linked.exe");
			Assert::AreEqual<uint64_t>(0, get64(image, get32(image, sectionHeader(image, ".text") + 20)), L"unresolved weak external defaults to zero");
		}

		TEST_METHOD(ResponseFileAndLinkerOptionsAreHonored)
		{
			Workspace workspace;
			workspace.write(L"input.obj", makeEntryObject());
			workspace.writeText(L"link.rsp", L"/OUT:\"linked.exe\" /ENTRY:main /MACHINE:X64 /SUBSYSTEM:WINDOWS,6.2 /BASE:0x180000000 /NXCOMPAT:NO /INCREMENTAL:NO /Nologo input.obj");
			assertSuccess(runLinker(workspace, { L"@" + workspace.file(L"link.rsp").wstring() }));
			const auto image = workspace.read(L"linked.exe");
			const auto optional = optionalHeader(image);
			Assert::AreEqual<uint64_t>(0x180000000, get64(image, optional + 24), L"/BASE");
			Assert::AreEqual<uint16_t>(2, get16(image, optional + 68), L"/SUBSYSTEM:WINDOWS");
			Assert::AreEqual<uint16_t>(6, get16(image, optional + 48), L"subsystem major version");
			Assert::AreEqual<uint16_t>(2, get16(image, optional + 50), L"subsystem minor version");
			Assert::IsTrue((get16(image, optional + 70) & 0x100) == 0, L"/NXCOMPAT:NO");
		}

		TEST_METHOD(GnuShortOptionsAndMapOptionAreAccepted)
		{
			Workspace workspace;
			workspace.write(L"input.obj", makeEntryObject());
			const auto output = workspace.file(L"short.exe").wstring();
			const auto result = runLinker(workspace, { L"-o" + output, L"-emain", L"/MAP", workspace.file(L"input.obj").wstring() });
			assertSuccess(result);
			Assert::IsTrue(fs::exists(workspace.file(L"short.map")), L"/MAP creates a map beside the executable");
		}

		TEST_METHOD(LtcgAndLlvmBitcodeInputsAreExplicitlyUnsupported)
		{
			Workspace rawWorkspace;
			const auto rawBitcode = Bytes{ 'B', 'C', 0xc0, 0xde, 0x00, 0x00, 0x00, 0x00 };
			rawWorkspace.write(L"input.obj", rawBitcode);
			auto args = outputArgs(rawWorkspace);
			args.push_back(rawWorkspace.file(L"input.obj").wstring());
			assertFailureContains(runLinker(rawWorkspace, args), "LLVM bitcode/LTCG input is not supported");
			assertNoOutput(rawWorkspace);

			Workspace wrappedWorkspace;
			const auto wrappedBitcode = Bytes{ 0xde, 0xc0, 0x17, 0x0b, 0x00, 0x00, 0x00, 0x00 };
			wrappedWorkspace.write(L"input.obj", wrappedBitcode);
			auto wrappedArgs = outputArgs(wrappedWorkspace);
			wrappedArgs.push_back(wrappedWorkspace.file(L"input.obj").wstring());
			assertFailureContains(runLinker(wrappedWorkspace, wrappedArgs), "LLVM bitcode/LTCG input is not supported");

			Workspace sectionWorkspace;
			const auto sectionObject = makeObject({ TestSection{ ".llvmbc", rawBitcode, 0x40000040 }, TestSection{ ".text",{ 0xc3 } } },
				{ TestSymbol{ "main", 0, 2, 2,{} } });
			assertFailureContains(linkObject(sectionWorkspace, sectionObject), "LLVM bitcode/LTCG section '.llvmbc' is not supported");

			Workspace archiveWorkspace;
			archiveWorkspace.write(L"input.lib", makeArchive({ { "module.obj", rawBitcode } }));
			assertFailureContains(runLinker(archiveWorkspace, { L"/OUT:" + archiveWorkspace.file(L"linked.exe").wstring(), archiveWorkspace.file(L"input.lib").wstring() }),
				"input.lib(module.obj): LLVM bitcode/LTCG input is not supported");
		}

		TEST_METHOD(MalformedTruncatedObjectReportsErrorAndWritesNoImage)
		{
			Workspace workspace;
			workspace.write(L"input.obj", Bytes{ 0x64, 0x86, 0x01 });
			auto args = outputArgs(workspace);
			args.push_back(workspace.file(L"input.obj").wstring());
			const auto result = runLinker(workspace, args);
			assertFailureContains(result, "truncated");
			assertNoOutput(workspace);
		}

		TEST_METHOD(NonAmd64ObjectIsRejected)
		{
			Workspace workspace;
			const auto result = linkObject(workspace, makeObject({ TestSection{ ".text",{ 0xc3 } } }, { TestSymbol{ "main", 0, 1, 2,{} } }, 0x14c));
			assertFailureContains(result, "expected AMD64 COFF object");
			assertNoOutput(workspace);
		}

		TEST_METHOD(UnresolvedExternalIsReported)
		{
			Workspace workspace;
			TestSection text{ ".text", Bytes(4, 0) };
			text.relocations = { { 0, 1, 4 } };
			const auto result = linkObject(workspace, makeObject({ text }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "notFound", 0, 0, 2,{} } }));
			assertFailureContains(result, "notFound");
			assertFailureContains(result, "referenced by");
			assertFailureContains(result, "input.obj");
			Assert::AreEqual<DWORD>(1, result.exitCode);
			assertNoOutput(workspace);
		}

		TEST_METHOD(UnresolvedDiagnosticsIdentifyAllReferencingObjects)
		{
			Workspace workspace;
			const auto object = [](const string& symbol) {
				TestSection text{ ".text", Bytes(4, 0) };
				text.relocations = { { 0, 0, 4 } };
				return makeObject({ text }, { TestSymbol{ symbol, 0, 0, 2,{} } });
			};
			workspace.write(L"entry.obj", makeEntryObject());
			workspace.write(L"first.obj", object("missingA"));
			workspace.write(L"second.obj", object("missingA"));
			workspace.write(L"third.obj", object("missingB"));
			const auto result = runLinker(workspace, { L"/ENTRY:main", L"/OUT:missing.exe", L"entry.obj", L"first.obj", L"second.obj", L"third.obj" });
			assertFailureContains(result, "'missingA' (referenced by first.obj, second.obj)");
			assertFailureContains(result, "'missingB' (referenced by third.obj)");
			Assert::AreEqual<DWORD>(1, result.exitCode);
			Assert::IsFalse(fs::exists(workspace.file(L"missing.exe")));
		}

		TEST_METHOD(UnsupportedAmd64RelocationIsReported)
		{
			Workspace workspace;
			TestSection text{ ".text", Bytes(8, 0) };
			text.relocations = { { 0, 0, 12 } };
			const auto result = linkObject(workspace, makeObject({ text }, { TestSymbol{ "main", 0, 1, 2,{} } }));
			assertFailureContains(result, "unsupported AMD64 relocation type");
		}

		TEST_METHOD(RelocationOutsideInitializedDataIsRejected)
		{
			Workspace workspace;
			TestSection text{ ".text",{ 0xc3 } };
			text.relocations = { { 1, 0, 4 } };
			const auto result = linkObject(workspace, makeObject({ text }, { TestSymbol{ "main", 0, 1, 2,{} } }));
			assertFailureContains(result, "relocation offset is outside its section");
		}

		TEST_METHOD(Addr32OverflowIsRejected)
		{
			Workspace workspace;
			TestSection data{ ".data", Bytes(4, 0), 0xc0000040 };
			data.relocations = { { 0, 0, 2 } };
			const auto object = makeObject({ data, TestSection{ ".text",{ 0xc3 } } }, { TestSymbol{ "main", 0, 2, 2,{} } });
			const auto result = linkObject(workspace, object);
			assertFailureContains(result, "does not fit its 32-bit relocation type");
		}

		TEST_METHOD(DuplicateStrongDefinitionsAreRejected)
		{
			Workspace workspace;
			workspace.write(L"first.obj", makeEntryObject());
			workspace.write(L"second.obj", makeEntryObject());
			auto args = outputArgs(workspace);
			args.push_back(workspace.file(L"first.obj").wstring());
			args.push_back(workspace.file(L"second.obj").wstring());
			const auto result = runLinker(workspace, args);
			assertFailureContains(result, "duplicate definition");
			assertFailureContains(result, "first.obj");
			assertFailureContains(result, "second.obj");
			Assert::AreEqual<DWORD>(1, result.exitCode);
			assertNoOutput(workspace);
		}

		TEST_METHOD(ComdatNoDuplicatesAndSizeMismatchAreRejected)
		{
			Workspace workspace;
			workspace.write(L"entry.obj", makeEntryObject());
			workspace.write(L"first.obj", makeComdatObject("duplicate", { 0x11 }, 1));
			workspace.write(L"second.obj", makeComdatObject("duplicate", { 0x11 }, 1));
			auto args = outputArgs(workspace);
			for (const auto& file : { L"entry.obj", L"first.obj", L"second.obj" }) args.push_back(workspace.file(file).wstring());
			assertFailureContains(runLinker(workspace, args), "duplicate NODUPLICATES COMDAT");
			Workspace sizeWorkspace;
			sizeWorkspace.write(L"entry.obj", makeEntryObject());
			sizeWorkspace.write(L"first.obj", makeComdatObject("sameSize", { 0x11 }, 3));
			sizeWorkspace.write(L"second.obj", makeComdatObject("sameSize", { 0x11, 0x22 }, 3));
			auto sizeArgs = outputArgs(sizeWorkspace);
			for (const auto& file : { L"entry.obj", L"first.obj", L"second.obj" }) sizeArgs.push_back(sizeWorkspace.file(file).wstring());
			assertFailureContains(runLinker(sizeWorkspace, sizeArgs), "SAME_SIZE COMDAT mismatch");
		}

		TEST_METHOD(ComdatExactMatchMismatchIsRejected)
		{
			Workspace workspace;
			workspace.write(L"entry.obj", makeEntryObject());
			workspace.write(L"first.obj", makeComdatObject("exact", { 0x11 }, 4));
			workspace.write(L"second.obj", makeComdatObject("exact", { 0x22 }, 4));
			auto args = outputArgs(workspace);
			for (const auto& file : { L"entry.obj", L"first.obj", L"second.obj" }) args.push_back(workspace.file(file).wstring());
			assertFailureContains(runLinker(workspace, args), "EXACT_MATCH COMDAT mismatch");
		}

		TEST_METHOD(InvalidWeakSearchModeIsRejected)
		{
			Workspace workspace;
			TestSymbol weak{ "weakRef", 0, 0, 105,{ weakAux(3, 4) } };
			TestSymbol fallback{ "fallback", 0, 0, 2,{} };
			const auto result = linkObject(workspace, makeObject({ TestSection{ ".text",{ 0xc3 } } }, { TestSymbol{ "main", 0, 1, 2,{} }, weak, fallback }));
			assertFailureContains(result, "unsupported weak-external search mode");
		}

		TEST_METHOD(MalformedArchiveAndMalformedImportObjectAreRejected)
		{
			Workspace workspace;
			workspace.write(L"bad.lib", Bytes{ 'n', 'o', 'p', 'e' });
			auto args = outputArgs(workspace);
			args.push_back(workspace.file(L"bad.lib").wstring());
			assertFailureContains(runLinker(workspace, args), "truncated");
			Workspace importWorkspace;
			TestSection text{ ".text",{ 0xff, 0x25, 0, 0, 0, 0 } };
			text.relocations = { { 2, 1, 4 } };
			importWorkspace.write(L"input.obj", makeObject({ text }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "__imp_bad", 0, 0, 2,{} } }));
			auto badImport = makeImportObject("bad", "BAD.dll", "bad", 0, 4);
			put16(badImport, 6, 0x14c);
			importWorkspace.write(L"bad.lib", makeArchive({ { "bad.obj", badImport } }));
			auto importArgs = outputArgs(importWorkspace);
			importArgs.push_back(importWorkspace.file(L"input.obj").wstring());
			importArgs.push_back(importWorkspace.file(L"bad.lib").wstring());
			assertFailureContains(runLinker(importWorkspace, importArgs), "import library member is not AMD64");
		}

		TEST_METHOD(MalformedAndUnsupportedCoffDirectivesAreReported)
		{
			Workspace workspace;
			const auto unknown = makeObject({ TestSection{ ".text",{ 0xc3 } }, makeDirectiveSection("-not-a-linker-option:value") },
				{ TestSymbol{ "main", 0, 1, 2,{} } });
			const auto unknownResult = linkObject(workspace, unknown);
			assertFailureContains(unknownResult, "unsupported .drectve option");
			assertNoOutput(workspace);

			Workspace alignmentWorkspace;
			const auto invalidAlignment = makeObject({ TestSection{ ".text",{ 0xc3 } }, makeDirectiveSection("-align:300") },
				{ TestSymbol{ "main", 0, 1, 2,{} } });
			assertFailureContains(linkObject(alignmentWorkspace, invalidAlignment), ".drectve /ALIGN must be a power of two");

			Workspace quoteWorkspace;
			const auto malformedQuote = makeObject({ TestSection{ ".text",{ 0xc3 } }, makeDirectiveSection("-defaultlib:\"unterminated") },
				{ TestSymbol{ "main", 0, 1, 2,{} } });
			assertFailureContains(linkObject(quoteWorkspace, malformedQuote), "unterminated quote in .drectve");

			Workspace exportWorkspace;
			const auto malformedExport = makeObject({ TestSection{ ".text",{ 0xc3 } }, makeDirectiveSection("-export:api,@1,@2") },
				{ TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "api", 0, 1, 2,{} } });
			assertFailureContains(linkObject(exportWorkspace, malformedExport), "specifies more than one ordinal");
		}

		TEST_METHOD(ResponseFilesHandleNestingEncodingAndCycles)
		{
			Workspace workspace;
			workspace.write(L"input file.obj", makeEntryObject());
			workspace.writeText(L"outer.rsp", L"@inner.rsp /OUT:first.exe");
			workspace.writeText(L"inner.rsp", L"/ENTRY:main \"input file.obj\" /OUT:earlier.exe");
			assertSuccess(runLinker(workspace, { L"@outer.rsp", L"/OUT:last.exe" }));
			Assert::IsTrue(fs::exists(workspace.file(L"last.exe")), L"the last output option wins across nested response files");
			Assert::IsFalse(fs::exists(workspace.file(L"earlier.exe")));
			assertSuccess(runLinker(workspace, { L"@inner.rsp", L"/OUT:map.exe", L"/MAP:explicit.map", L"/MAP" }));
			Assert::IsTrue(fs::exists(workspace.file(L"map.map")));
			Assert::IsFalse(fs::exists(workspace.file(L"explicit.map")));
			workspace.writeText(L"commands.lnk", L"/ENTRY:main /OUT:lnk.exe \"input file.obj\"");
			assertSuccess(runExecutable(workspace, jwlink2Path(), { L"@commands" }, { { L"commands", L"/UNKNOWN" } }));
			Assert::IsTrue(fs::exists(workspace.file(L"lnk.exe")), L"the .lnk file wins over an environment variable");
			const string command = "/ENTRY:main /OUT:utf8.exe \"input file.obj\"";
			Bytes utf8{ 0xef, 0xbb, 0xbf };
			utf8.insert(utf8.end(), command.begin(), command.end());
			workspace.write(L"utf8.rsp", utf8);
			assertSuccess(runLinker(workspace, { L"@utf8.rsp" }));
			Assert::IsTrue(fs::exists(workspace.file(L"utf8.exe")));
			const string command16 = "/ENTRY:main /OUT:utf16.exe \"input file.obj\"";
			Bytes utf16{ 0xff, 0xfe };
			for (const auto ch : command16) { utf16.push_back(static_cast<uint8_t>(ch)); utf16.push_back(0); }
			workspace.write(L"utf16.rsp", utf16);
			assertSuccess(runLinker(workspace, { L"@utf16.rsp" }));
			Assert::IsTrue(fs::exists(workspace.file(L"utf16.exe")));
			workspace.write(L"invalid.rsp", Bytes{ 0xef, 0xbb, 0xbf, 0xff });
			assertFailureContains(runLinker(workspace, { L"@invalid.rsp" }), "invalid UTF-8");
			workspace.write(L"odd.rsp", Bytes{ 0xff, 0xfe, 0x41 });
			assertFailureContains(runLinker(workspace, { L"@odd.rsp" }), "invalid UTF-16LE");
			workspace.writeText(L"cycle.rsp", L"@CYCLE.RSP");
			assertFailureContains(runLinker(workspace, { L"@cycle.rsp" }), "cycle involving");
		}

		TEST_METHOD(JwlinkCommandEnvironmentExpansionAndLibrarySearch)
		{
			Workspace workspace;
			workspace.write(L"input.obj", makeObject({ TestSection{ ".text",{ 0xc3 } } }, { TestSymbol{ "main", 0, 1, 2,{} }, TestSymbol{ "needed", 0, 0, 2,{} } }));
			for (const auto* dir : { L"first", L"second", L"env" }) fs::create_directories(workspace.file(dir));
			auto archive = [](uint8_t marker) { return makeArchive({ { "lib.obj", makeObject({ TestSection{ ".text",{ marker, 0xc3 } } }, { TestSymbol{ "needed", 0, 1, 2,{} } }) } }); };
			workspace.write(L"first\\x.lib", archive(0x71));
			workspace.write(L"second\\x.lib", archive(0x72));
			workspace.write(L"env\\x.lib", archive(0x73));
			const vector<pair<wstring, wstring>> env{ { L"LIBPE", L"" }, { L"LIB386", L"" }, { L"LIB", workspace.file(L"env").wstring() }, { L"JWL2_CMDS", L"format win pe option quiet,start=main file input.obj library x.lib name env.exe" }, { L"JWL2_DIR", L"format win pe" } };
			assertSuccess(runExecutable(workspace, jwlink2Path(), { L"@JWL2_CMDS" }, env));
			const auto fromLib = workspace.read(L"env.exe");
			Assert::AreEqual<uint8_t>(0x73, fromLib[get32(fromLib, sectionHeader(fromLib, ".text") + 20) + 1], L"LIB provides libraries when no explicit path matches");
			assertSuccess(runExecutable(workspace, jwlink2Path(), { L"%JWL2_DIR%", L"option", L"start=main", L"file", L"input.obj", L"libpath", L"first", L"libpath", L"second", L"library", L"x.lib", L"name", L"order.exe" }, env));
			const auto image = workspace.read(L"order.exe");
			const auto raw = get32(image, sectionHeader(image, ".text") + 20);
			Assert::AreEqual<uint8_t>(0x72, image[raw + 1], L"last LIBPATH wins over earlier paths and LIB");
			const vector<pair<wstring, wstring>> orderedEnv{ { L"LIBPE", workspace.file(L"first").wstring() }, { L"LIB386", workspace.file(L"second").wstring() }, { L"LIB", workspace.file(L"env").wstring() } };
			assertSuccess(runExecutable(workspace, jwlink2Path(), { L"format", L"win", L"pe", L"option", L"start=main", L"file", L"input.obj", L"library", L"x.lib", L"name", L"envorder.exe" }, orderedEnv));
			const auto envImage = workspace.read(L"envorder.exe");
			Assert::AreEqual<uint8_t>(0x71, envImage[get32(envImage, sectionHeader(envImage, ".text") + 20) + 1], L"LIBPE precedes LIB386 and LIB");
			assertSuccess(runExecutable(workspace, jwlink2Path(), { L"/ENTRY:main", L"/OUT:switchorder.exe", L"/LIBPATH:first", L"/LIBPATH:second", L"input.obj", L"x.lib" }, orderedEnv));
			const auto switchImage = workspace.read(L"switchorder.exe");
			Assert::AreEqual<uint8_t>(0x71, switchImage[get32(switchImage, sectionHeader(switchImage, ".text") + 20) + 1], L"switch-style paths retain command-line order");
			assertSuccess(runExecutable(workspace, jwlink2Path(), { L"/ENTRY:main", L"/OUT:explicit.exe", L"/LIBPATH:first", L"input.obj", L"second\\x.lib" }, orderedEnv));
			const auto explicitImage = workspace.read(L"explicit.exe");
			Assert::AreEqual<uint8_t>(0x72, explicitImage[get32(explicitImage, sectionHeader(explicitImage, ".text") + 20) + 1], L"path-qualified libraries bypass search paths");
			workspace.write(L"x.lib", archive(0x74));
			assertSuccess(runExecutable(workspace, jwlink2Path(), { L"format", L"win", L"pe", L"option", L"start=main", L"file", L"input.obj", L"libpath", L"first", L"library", L"x.lib", L"name", L"current.exe" }, orderedEnv));
			const auto currentImage = workspace.read(L"current.exe");
			Assert::AreEqual<uint8_t>(0x74, currentImage[get32(currentImage, sectionHeader(currentImage, ".text") + 20) + 1], L"current directory precedes library paths");
			workspace.write(L"plain.obj", makeEntryObject());
			assertSuccess(runExecutable(workspace, jwlinkPath(), { L"%JWL2_DIR%", L"option", L"quiet,start=main", L"file", L"plain.obj", L"name", L"original.exe" }, env));
			Assert::IsTrue(fs::exists(workspace.file(L"original.exe")), L"JWlink accepts the same percent environment expansion");
			const vector<pair<wstring, wstring>> sourceEnv{ { L"JWL2_SOURCE", L"format win pe option quiet,start=main file plain.obj name originalenv.exe" } };
			assertSuccess(runExecutable(workspace, jwlinkPath(), { L"@JWL2_SOURCE" }, sourceEnv));
			Assert::IsTrue(fs::exists(workspace.file(L"originalenv.exe")), L"JWlink accepts environment command sources");
			const vector<pair<wstring, wstring>> cycle{ { L"JWL2_CYCLE", L"@JWL2_CYCLE" } };
			assertFailureContains(runExecutable(workspace, jwlink2Path(), { L"@JWL2_CYCLE" }, cycle), "cycle involving");
			assertFailureContains(runExecutable(workspace, jwlink2Path(), { L"%JWL2_MISSING%" }, { { L"JWL2_MISSING", L"" } }), "not defined");
		}

		TEST_METHOD(CommandLineAndResponseFileErrorsAreReported)
		{
			Workspace workspace;
			workspace.write(L"input.obj", makeEntryObject());
			assertFailureContains(runLinker(workspace, { L"/UNKNOWN", workspace.file(L"input.obj").wstring() }), "unsupported option");
			assertFailureContains(runLinker(workspace, { L"/MANIFESTINPUT:app.manifest", workspace.file(L"input.obj").wstring() }), "app.manifest");
			assertFailureContains(runLinker(workspace, { L"/BASE:0x12345", workspace.file(L"input.obj").wstring() }), "64-KiB-aligned");
			assertFailureContains(runLinker(workspace, { L"/ALIGN:300", workspace.file(L"input.obj").wstring() }), "/ALIGN must be a power of two");
			assertFailureContains(runLinker(workspace, { L"/SUBSYSTEM:EFI", workspace.file(L"input.obj").wstring() }), "unsupported subsystem");
			assertFailureContains(runLinker(workspace, { L"/MACHINE:X86", workspace.file(L"input.obj").wstring() }), "only supports /MACHINE:X64");
			assertFailureContains(runLinker(workspace, { L"/INCREMENTAL", workspace.file(L"input.obj").wstring() }), "INCREMENTAL:NO");
			assertFailureContains(runLinker(workspace, { L"@missing.rsp" }), "cannot open response file");
			workspace.writeText(L"bad.rsp", L"/OUT:\"unterminated");
			assertFailureContains(runLinker(workspace, { L"@" + workspace.file(L"bad.rsp").wstring() }), "unterminated quote");
		}

		TEST_METHOD(MissingInputsAndMissingFilesAreReported)
		{
			Workspace workspace;
			assertFailureContains(runLinker(workspace, {}), "no input object files or libraries");
			assertFailureContains(runLinker(workspace, { L"missing.obj" }), "cannot open");
			const vector<wstring> args{ L"format", L"win", L"pe", L"option", L"quiet,start=main", L"file", L"missing.obj", L"name", L"missing.exe" };
			const auto jwlink = runExecutable(workspace, jwlinkPath(), args);
			const auto jwlink2 = runLinker(workspace, args);
			Assert::AreEqual<DWORD>(1, jwlink.exitCode, L"JWlink reports a link failure with exit code 1");
			Assert::AreEqual(jwlink.exitCode, jwlink2.exitCode, L"JWlink2 reports the same failure status");
			assertFailureContains(jwlink2, "missing.obj");
			Assert::IsFalse(fs::exists(workspace.file(L"missing.exe")));
		}
	};
}
