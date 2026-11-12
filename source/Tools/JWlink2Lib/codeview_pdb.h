#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <string>
#include <vector>

using namespace std;

namespace JWlink2::codeview
{
	struct PdbIdentity
	{
		// The 16 bytes are written verbatim in the GUID field of the PDB info stream.
		array<uint8_t, 16> guid{};
		uint32_t age = 1;
		uint32_t signature = 0;
	};

	struct LinkedSection
	{
		uint16_t index = 0; // Final PE section number, one-based and contiguous.
		string name;
		uint32_t rva = 0; // Final image RVA.
		uint32_t size = 0;
		uint32_t characteristics = 0;
	};

	struct LinkedSymbol
	{
		string name;
		uint16_t section = 0;
		uint32_t offset = 0;
		uint32_t flags = 0;
	};

	struct ObjectSectionContribution
	{
		uint16_t sourceSection = 0;
		uint32_t sourceOffset = 0; // Half-open source range [sourceOffset, sourceOffset + size).
		uint32_t size = 0;
		uint16_t linkedSection = 0;
		uint32_t linkedOffset = 0; // Destination for sourceOffset after COMDAT/layout decisions.
	};

	struct TypeIndexPatch
	{
		// Byte offset from the beginning of a complete CodeView record to a u32 index.
		size_t offset = 0;
	};

	struct CodeViewTypeRecord
	{
		uint32_t originalIndex = 0;
		vector<uint8_t> record; // Includes CodeView's u16 length and u16 leaf kind.
		vector<TypeIndexPatch> references;
	};

	struct CodeViewAddressPatch
	{
		size_t offsetField = 0;  // u32 section-relative offset in the record.
		size_t sectionField = 0; // u16 PE section number in the record.
	};

	struct CodeViewSymbolRecord
	{
		uint16_t sourceSection = 0;
		vector<uint8_t> record; // Includes CodeView's u16 length and u16 symbol kind.
		vector<TypeIndexPatch> typeReferences;
		vector<CodeViewAddressPatch> addresses;
	};

	struct CodeViewSubsection
	{
		uint32_t kind = 0; // C13 DEBUG_S_* subsection kind.
		vector<uint8_t> data; // Payload only; kind/length/alignment are written by the builder.
	};

	struct CodeViewObject
	{
		string objectId; // Must uniquely identify this object in the builder.
		string moduleName;
		string objectFileName;
		vector<ObjectSectionContribution> contributions;
		vector<CodeViewTypeRecord> types;
		vector<CodeViewSymbolRecord> symbols;
		vector<CodeViewSubsection> subsections;
	};

	class PdbBuilder
	{
	public:
		explicit PdbBuilder(PdbIdentity identity);

		void addLinkedSection(LinkedSection section);
		void addLinkedSymbol(LinkedSymbol symbol);
		void addCodeViewObject(CodeViewObject object);

		// Emits an MSF 7.0 container containing PDB info, TPI, DBI, module symbol
		// streams and CodeView C13 subsections. Invalid or unsupported input throws
		// runtime_error; no placeholder or non-PDB output is written.
		void write(const filesystem::path& path) const;

	private:
		PdbIdentity identity_;
		vector<LinkedSection> sections_;
		vector<LinkedSymbol> symbols_;
		vector<CodeViewObject> objects_;
	};

} // namespace jwlink2::codeview
